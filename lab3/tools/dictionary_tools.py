#!/usr/bin/env python3
"""Validate Lab 3 dictionaries and run the public dictionary self-check."""

from __future__ import annotations

import argparse
import os
import re
import shutil
import stat
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import FrozenSet, Optional


DEFAULT_DICTIONARY_DIR = Path("fuzzing_dict")
DEFAULT_MAX_FILES = 4096
DEFAULT_MAX_TOTAL_BYTES = 4 * 1024 * 1024
DEFAULT_MAX_ENTRY_BYTES = 1024
DEFAULT_TIMEOUT_SECONDS = 20
MAX_POSITION_HINT = (1 << 64) - 1
LAB_ROOT = Path(__file__).resolve().parent.parent

DictionaryEntry = tuple[bytes, Optional[int]]

_ENTRY_NAME = re.compile(r"entry_[0-9]+(?:@([0-9]+))?\Z", re.ASCII)


class DictionaryFormatError(RuntimeError):
    """The on-disk dictionary does not meet the public format contract."""


class DictionaryCheckError(RuntimeError):
    """The public dictionary analysis could not be checked."""


def _dictionary_paths(directory: Path, max_files: int) -> list[tuple[Path, int]]:
    """Return validated dictionary paths and parsed hints in bytewise order."""
    if max_files < 1:
        raise ValueError("max_files must be positive")

    try:
        directory_mode = directory.lstat().st_mode
    except OSError as error:
        raise DictionaryFormatError("dictionary directory is missing") from error
    if not stat.S_ISDIR(directory_mode):
        raise DictionaryFormatError(
            "dictionary path must be a directory, not a link or regular file"
        )

    paths: list[tuple[Path, int]] = []
    try:
        for entry_count, path in enumerate(directory.iterdir(), start=1):
            if entry_count > max_files:
                raise DictionaryFormatError(
                    f"dictionary contains more than {max_files} entries"
                )
            match = _ENTRY_NAME.fullmatch(path.name)
            if match is None:
                raise DictionaryFormatError(
                    f"malformed dictionary filename: {path.name!r}"
                )
            try:
                mode = path.lstat().st_mode
            except OSError as error:
                raise DictionaryFormatError(
                    f"cannot inspect dictionary entry: {path.name!r}"
                ) from error
            if not stat.S_ISREG(mode):
                raise DictionaryFormatError(
                    "dictionary entries must be direct regular files: "
                    f"{path.name!r}"
                )
            hint = int(match.group(1)) if match.group(1) is not None else -1
            if hint > MAX_POSITION_HINT:
                raise DictionaryFormatError(
                    f"dictionary position hint is out of range: {path.name!r}"
                )
            paths.append((path, hint))
    except DictionaryFormatError:
        raise
    except OSError as error:
        raise DictionaryFormatError("cannot enumerate dictionary directory") from error

    paths.sort(key=lambda item: os.fsencode(item[0].name))
    return paths


def _read_entry(path: Path, max_entry_bytes: int) -> bytes:
    """Read one bounded regular file without following a final-component link."""
    if max_entry_bytes < 1:
        raise ValueError("max_entry_bytes must be positive")

    flags = os.O_RDONLY
    if hasattr(os, "O_CLOEXEC"):
        flags |= os.O_CLOEXEC
    if hasattr(os, "O_NOFOLLOW"):
        flags |= os.O_NOFOLLOW

    descriptor: Optional[int] = None
    try:
        descriptor = os.open(path, flags)
        entry_stat = os.fstat(descriptor)
        if not stat.S_ISREG(entry_stat.st_mode):
            raise DictionaryFormatError(
                f"dictionary entry is not a regular file: {path.name!r}"
            )
        if entry_stat.st_size == 0:
            raise DictionaryFormatError(
                f"dictionary entry is empty: {path.name!r}"
            )
        if entry_stat.st_size > max_entry_bytes:
            raise DictionaryFormatError(
                f"dictionary entry exceeds {max_entry_bytes} bytes: {path.name!r}"
            )
        with os.fdopen(descriptor, "rb") as entry_file:
            descriptor = None
            contents = entry_file.read(max_entry_bytes + 1)
    except DictionaryFormatError:
        raise
    except OSError as error:
        raise DictionaryFormatError(
            f"cannot read dictionary entry: {path.name!r}"
        ) from error
    finally:
        if descriptor is not None:
            os.close(descriptor)

    if not contents:
        raise DictionaryFormatError(f"dictionary entry is empty: {path.name!r}")
    if len(contents) > max_entry_bytes:
        raise DictionaryFormatError(
            f"dictionary entry exceeds {max_entry_bytes} bytes: {path.name!r}"
        )
    return contents


def load_dictionary(
    directory: Path,
    *,
    max_files: int = DEFAULT_MAX_FILES,
    max_total_bytes: int = DEFAULT_MAX_TOTAL_BYTES,
    max_entry_bytes: int = DEFAULT_MAX_ENTRY_BYTES,
) -> FrozenSet[DictionaryEntry]:
    """Normalize an on-disk dictionary to ``(raw bytes, optional hint)`` pairs.

    Filenames and enumeration order carry no semantic meaning. Duplicate pairs
    are rejected instead of being silently collapsed.
    """
    if max_total_bytes < 1:
        raise ValueError("max_total_bytes must be positive")

    entries: set[DictionaryEntry] = set()
    total_bytes = 0
    for path, parsed_hint in _dictionary_paths(Path(directory), max_files):
        contents = _read_entry(path, max_entry_bytes)
        total_bytes += len(contents)
        if total_bytes > max_total_bytes:
            raise DictionaryFormatError(
                f"dictionary exceeds the {max_total_bytes}-byte total limit"
            )
        hint: Optional[int] = None if parsed_hint < 0 else parsed_hint
        entry = (contents, hint)
        if entry in entries:
            raise DictionaryFormatError("dictionary contains a duplicate semantic entry")
        entries.add(entry)

    return frozenset(entries)


# These values are deliberately public: they correspond to dictionary_public.ll
# and make the self-check useful while keeping private evaluation values separate.
PUBLIC_EXPECTATIONS: dict[str, FrozenSet[DictionaryEntry]] = {
    "flat global bytes": frozenset({(b"A\x00B\xff", None)}),
    "hinted byte comparison": frozenset({(b"Z", 5)}),
    "ordered integer boundaries": frozenset(
        {
            (b"\x01\x01", None),
            (b"\x02\x01", None),
            (b"\x03\x01", None),
        }
    ),
    "switch cases": frozenset({(b"\x44\x33\x22\x11", None)}),
    "strcmp": frozenset({(b"PUBLIC!", 2)}),
    "strncmp prefix": frozenset({(b"PRE", 8)}),
    "memcmp binary slice": frozenset({(b"\x00Q\xff", 16)}),
    "strstr needle": frozenset({(b"NEEDLE", None)}),
}


def missing_public_categories(
    entries: FrozenSet[DictionaryEntry],
) -> list[str]:
    """Return category names whose representative semantic entries are absent."""
    return [
        category
        for category, required in PUBLIC_EXPECTATIONS.items()
        if not required.issubset(entries)
    ]


def run_public_check(
    plugin: Path,
    fixture: Path,
    *,
    opt: str = "opt",
    timeout_seconds: int = DEFAULT_TIMEOUT_SECONDS,
) -> FrozenSet[DictionaryEntry]:
    """Run the standalone pass on the public fixture and check its dictionary."""
    if timeout_seconds < 1:
        raise ValueError("timeout_seconds must be positive")
    opt_path = shutil.which(opt)
    if opt_path is None:
        raise DictionaryCheckError(f"LLVM optimizer {opt!r} was not found")

    plugin = Path(plugin).resolve()
    fixture = Path(fixture).resolve()
    if not plugin.is_file():
        raise DictionaryCheckError("dictionary pass plugin was not built")
    if not fixture.is_file():
        raise DictionaryCheckError("public dictionary fixture is missing")

    command = [
        opt_path,
        f"-load-pass-plugin={plugin}",
        "-passes=BuildDictionary",
        "-disable-output",
        os.fspath(fixture),
    ]
    with tempfile.TemporaryDirectory(prefix="cis547-lab3-dictionary-") as temporary:
        try:
            completed = subprocess.run(
                command,
                cwd=temporary,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
                timeout=timeout_seconds,
                check=False,
            )
        except subprocess.TimeoutExpired as error:
            raise DictionaryCheckError("dictionary analysis timed out") from error
        except OSError as error:
            raise DictionaryCheckError("dictionary analysis could not be run") from error
        if completed.returncode != 0:
            raise DictionaryCheckError("dictionary analysis did not complete")

        try:
            entries = load_dictionary(Path(temporary) / DEFAULT_DICTIONARY_DIR)
        except DictionaryFormatError as error:
            # Do not echo raw bytes or tool output. The detailed exception is
            # retained as the cause for local debugging.
            raise DictionaryCheckError(
                "dictionary output does not satisfy the public format contract"
            ) from error

    missing = missing_public_categories(entries)
    if missing:
        raise DictionaryCheckError(
            "dictionary is missing public categories: " + ", ".join(missing)
        )
    return entries


def _parse_args(arguments: Optional[list[str]]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate Lab 3 dictionary output or run its public self-check."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    validate = subparsers.add_parser(
        "validate", help="validate and normalize an existing fuzzing_dict directory"
    )
    validate.add_argument(
        "directory", nargs="?", type=Path, default=DEFAULT_DICTIONARY_DIR
    )

    check = subparsers.add_parser(
        "check", help="run BuildDictionary on the public LLVM fixture"
    )
    check.add_argument(
        "--plugin", type=Path, default=LAB_ROOT / "build/FuzzingAnalysis.so"
    )
    check.add_argument(
        "--fixture", type=Path, default=LAB_ROOT / "test/dictionary_public.ll"
    )
    check.add_argument("--opt", default=os.environ.get("OPT", "opt"))
    check.add_argument(
        "--timeout", type=int, default=DEFAULT_TIMEOUT_SECONDS, metavar="SECONDS"
    )
    return parser.parse_args(arguments)


def main(arguments: Optional[list[str]] = None) -> int:
    args = _parse_args(arguments)
    try:
        if args.command == "validate":
            entries = load_dictionary(args.directory)
            print(f"Valid dictionary: {len(entries)} semantic entries")
        else:
            entries = run_public_check(
                args.plugin,
                args.fixture,
                opt=args.opt,
                timeout_seconds=args.timeout,
            )
            print(
                "Public dictionary self-check passed: "
                f"{len(PUBLIC_EXPECTATIONS)} categories, {len(entries)} entries"
            )
    except (DictionaryFormatError, DictionaryCheckError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
