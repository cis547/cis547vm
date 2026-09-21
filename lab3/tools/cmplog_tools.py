#!/usr/bin/env python3
"""Validate bounded comparison logs and execute Lab 3's public runtime check."""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import re
import shutil
import stat
import subprocess
import sys
import tempfile
from typing import NamedTuple, Sequence


MAX_RECORDS = 4096
MAX_OPERAND_BYTES = 64
MAX_LOG_BYTES = 1_200_000
DEFAULT_TIMEOUT_SECONDS = 10
LAB_ROOT = Path(__file__).resolve().parent.parent
_HEX = re.compile(rb"(?:[0-9a-f]{2}){1,64}\Z")


class ComparisonRecord(NamedTuple):
    kind: str
    width: int
    lhs: bytes
    rhs: bytes


class ComparisonFormatError(RuntimeError):
    """A comparison log violates its public format or resource bounds."""


class ComparisonCheckError(RuntimeError):
    """A value-free status describing a build or execution failure."""


class RunObservation(NamedTuple):
    records: tuple[ComparisonRecord, ...]
    stdout: bytes
    returncode: int


def load_comparison_log(
    path: Path, *, max_records: int = MAX_RECORDS, max_bytes: int = MAX_LOG_BYTES
) -> tuple[ComparisonRecord, ...]:
    """Read unique paired records without following links or opening a FIFO.

    Empty files are valid. Empty operands use ``-``; all other operands use
    lowercase, even-length hex. Integer bytes are canonical little-endian.
    """
    if max_records < 1 or max_bytes < 1:
        raise ValueError("comparison log bounds must be positive")
    path = Path(path)
    descriptor = None
    try:
        before = path.lstat()
        if not stat.S_ISREG(before.st_mode):
            raise ComparisonFormatError("comparison log must be a direct regular file")
        flags = os.O_RDONLY | getattr(os, "O_CLOEXEC", 0)
        flags |= getattr(os, "O_NOFOLLOW", 0) | getattr(os, "O_NONBLOCK", 0)
        descriptor = os.open(path, flags)
        current = os.fstat(descriptor)
        if (
            not stat.S_ISREG(current.st_mode)
            or (before.st_dev, before.st_ino) != (current.st_dev, current.st_ino)
            or current.st_size > max_bytes
        ):
            raise ComparisonFormatError("invalid or oversized comparison log")
        with os.fdopen(descriptor, "rb") as source:
            descriptor = None
            data = source.read(max_bytes + 1)
    except ComparisonFormatError:
        raise
    except OSError as error:
        raise ComparisonFormatError("comparison log could not be read") from error
    finally:
        if descriptor is not None:
            os.close(descriptor)
    if len(data) > max_bytes:
        raise ComparisonFormatError("comparison log exceeds its byte limit")
    if data and not data.endswith(b"\n"):
        raise ComparisonFormatError("comparison log has an incomplete record")

    records: list[ComparisonRecord] = []
    seen: set[ComparisonRecord] = set()
    for line in data.split(b"\n")[:-1]:
        if len(records) >= max_records:
            raise ComparisonFormatError("comparison log exceeds its record limit")
        fields = line.split(b"\t")
        if len(fields) != 4:
            raise ComparisonFormatError("malformed comparison record")
        kind, width, *operands = fields
        if kind == b"icmp":
            if width not in (b"8", b"16", b"32", b"64"):
                raise ComparisonFormatError("invalid integer comparison width")
        elif kind in (b"strcmp", b"strncmp", b"memcmp"):
            if width != b"0":
                raise ComparisonFormatError("invalid routine comparison width")
        else:
            raise ComparisonFormatError("unsupported comparison kind")
        decoded = []
        for operand in operands:
            if operand == b"-":
                decoded.append(b"")
            elif _HEX.fullmatch(operand):
                decoded.append(bytes.fromhex(operand.decode("ascii")))
            else:
                raise ComparisonFormatError("malformed comparison operand")
        bits = int(width)
        if kind == b"icmp" and any(len(value) != bits // 8 for value in decoded):
            raise ComparisonFormatError("integer operand size does not match its width")
        record = ComparisonRecord(kind.decode("ascii"), bits, *decoded)
        if record in seen:
            raise ComparisonFormatError("duplicate comparison record")
        seen.add(record)
        records.append(record)
    return tuple(records)


# The driver is compiled separately, so its comparisons are never instrumented.
# Fixtures see fixed binary stdin plus a zero-filled tail, and return a value
# whose printed bytes and exit code must survive instrumentation unchanged.
DEFAULT_DRIVER = r"""
#include <cstdio>
extern "C" int target(const unsigned char *, unsigned long long);
int main() {
  unsigned char input[512] = {};
  const auto size = std::fread(input, 1, 256, stdin);
  if (std::ferror(stdin)) return 111;
  const int result = target(input, size);
  std::printf("%d\n", result);
  return static_cast<unsigned int>(result) & 3U;
}
"""


def _quiet_command(command: list[str], cwd: Path, timeout: int, status: str) -> None:
    try:
        completed = subprocess.run(
            command, cwd=cwd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            timeout=timeout, check=False,
        )
    except subprocess.TimeoutExpired as error:
        reason = "analysis_timeout" if status == "analysis_failed" else status
        raise ComparisonCheckError(reason) from error
    except OSError as error:
        raise ComparisonCheckError(status) from error
    if completed.returncode != 0:
        raise ComparisonCheckError(status)


def _execute(executable: Path, data: bytes, cwd: Path, env: dict[str, str], timeout: int):
    output = cwd / "stdout.bin"
    try:
        with output.open("wb") as stdout:
            result = subprocess.run(
                [str(executable)], input=data, cwd=cwd, env=env, stdout=stdout,
                stderr=subprocess.DEVNULL, timeout=timeout, check=False,
            )
        if result.returncode < 0 or output.stat().st_size > 1024:
            raise ComparisonCheckError("runtime_execution_failed")
        return output.read_bytes(), result.returncode
    except subprocess.TimeoutExpired as error:
        raise ComparisonCheckError("runtime_timeout") from error
    except OSError as error:
        raise ComparisonCheckError("runtime_execution_failed") from error


def run_fixture(
    plugin: Path,
    runtime: Path,
    fixture: Path,
    inputs: Sequence[bytes],
    *,
    opt: str = "opt",
    cxx: str = "clang++",
    timeout_seconds: int = DEFAULT_TIMEOUT_SECONDS,
    driver_source: str = DEFAULT_DRIVER,
) -> list[RunObservation]:
    """Build a fixture twice, then check execution preservation and fresh logs.

    Only the fixture IR passes through ``ComparisonLogging``. The original and
    instrumented executables use the same uninstrumented driver and runtime.
    Reusing one log path across runs checks process-local truncation and reset.
    """
    if timeout_seconds < 1:
        raise ValueError("timeout must be positive")
    plugin, runtime, fixture = (Path(path).resolve() for path in (plugin, runtime, fixture))
    for path, status in ((plugin, "missing_plugin"), (runtime, "missing_source"),
                         (fixture, "missing_fixture")):
        if not path.is_file():
            raise ComparisonCheckError(status)
    optimizer, compiler = shutil.which(opt), shutil.which(cxx)
    if optimizer is None or compiler is None:
        raise ComparisonCheckError("checker_unavailable")
    with tempfile.TemporaryDirectory(prefix="lab3-runtime-check-") as temporary:
        temp = Path(temporary)
        transformed = temp / "instrumented.ll"
        driver = temp / "driver.cpp"
        driver.write_text(driver_source, encoding="utf-8")
        _quiet_command(
            [optimizer, f"-load-pass-plugin={plugin}", "-passes=ComparisonLogging",
             "-S", str(fixture), "-o", str(transformed)],
            temp, timeout_seconds, "analysis_failed",
        )
        original, instrumented = temp / "original", temp / "instrumented"
        for source, executable in ((fixture, original), (transformed, instrumented)):
            _quiet_command(
                [compiler, "-O0", str(source), str(driver), "-Wl,--no-as-needed",
                 str(runtime), f"-Wl,-rpath,{runtime.parent}", "-o", str(executable)],
                temp, timeout_seconds, "runtime_build_failed",
            )
        log = temp / "comparisons.log"
        # A runtime must initialize an enabled log even before its first hook.
        log.write_bytes(b"stale data from a previous process\n")
        environment = dict(os.environ)
        environment.pop("LAB3_CMPLOG_PATH", None)
        logged_environment = dict(environment, LAB3_CMPLOG_PATH=str(log))
        observations = []
        for data in inputs:
            baseline = _execute(original, data, temp, environment, timeout_seconds)
            observed = _execute(instrumented, data, temp, logged_environment, timeout_seconds)
            if observed != baseline:
                raise ComparisonCheckError("invalid_runtime_semantics")
            try:
                records = load_comparison_log(log)
            except ComparisonFormatError as error:
                raise ComparisonCheckError("invalid_comparison_log") from error
            observations.append(RunObservation(records, *observed))
        return observations


PUBLIC_INPUT = b"\x81AB\x00C\xfe"
PUBLIC_EXPECTED = frozenset({
    ComparisonRecord("icmp", 8, b"\x81", b"\xa5"),
    ComparisonRecord("strcmp", 0, b"AB", b"ABC"),
    ComparisonRecord("strncmp", 0, b"AB", b"ABC"),
    ComparisonRecord("memcmp", 0, b"\x00C\xfe", b"\x00C\xff"),
})


def run_public_check(plugin: Path, runtime: Path, fixture: Path, **kwargs):
    observations = run_fixture(plugin, runtime, fixture, [PUBLIC_INPUT], **kwargs)
    actual = frozenset(observations[0].records)
    if actual != PUBLIC_EXPECTED:
        status = "missing_runtime_semantics" if not PUBLIC_EXPECTED.issubset(actual) else "invalid_runtime_semantics"
        raise ComparisonCheckError(status)
    return observations[0].records


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    check = commands.add_parser("check", help="execute the public comparison logging fixture")
    check.add_argument("--plugin", type=Path, required=True)
    check.add_argument("--runtime", type=Path, required=True)
    check.add_argument("--fixture", type=Path, default=LAB_ROOT / "test/cmplog_public.ll")
    check.add_argument("--opt", default="opt")
    check.add_argument("--cxx", default="clang++")
    check.add_argument("--timeout", type=int, default=DEFAULT_TIMEOUT_SECONDS)
    args = parser.parse_args(argv)
    if args.timeout < 1:
        parser.error("--timeout must be positive")
    try:
        records = run_public_check(args.plugin, args.runtime, args.fixture,
                                  opt=args.opt, cxx=args.cxx, timeout_seconds=args.timeout)
    except ComparisonCheckError as error:
        print(f"Comparison logging check failed: {error}", file=sys.stderr)
        return 1
    print(f"Comparison logging check passed ({len(records)} paired records).")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
