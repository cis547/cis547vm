#!/usr/bin/env python3
"""Load and validate submitted seeds for Lab 3's public JSON target."""

from __future__ import annotations

import argparse
import os
import shutil
import stat
import subprocess
import sys
import tempfile
from pathlib import Path


DEFAULT_SEED_DIR = Path("json_seeds")
DEFAULT_MAX_SEEDS = 30
DEFAULT_MAX_SEED_BYTES = 64 * 1024
DEFAULT_MAX_DIRECTORY_ENTRIES = 256
VALIDATOR_TIMEOUT_SECONDS = 10
LAB_ROOT = Path(__file__).resolve().parent.parent


class SeedDirectoryError(RuntimeError):
    """The submitted JSON seed directory does not meet the assignment contract."""


class JsonValidatorError(RuntimeError):
    """The exact public JSON validator could not check the submitted seeds."""


def _sorted_seed_paths(
    seed_dir: Path, max_seeds: int, max_entries: int
) -> list[Path]:
    """Return the flat, visible JSON files in deterministic filename order."""
    if max_seeds < 1:
        raise ValueError("max_seeds must be positive")
    if max_entries < max_seeds:
        raise ValueError("max_entries must be at least max_seeds")
    if seed_dir.is_symlink():
        raise SeedDirectoryError(
            f"seed directory must not be a symbolic link: {seed_dir}"
        )
    if not seed_dir.is_dir():
        raise SeedDirectoryError(f"missing seed directory: {seed_dir}")

    entries: list[Path] = []
    try:
        for entry_count, entry in enumerate(seed_dir.iterdir(), start=1):
            if entry_count > max_entries:
                raise SeedDirectoryError(
                    f"seed directory contains more than {max_entries} total entries"
                )
            if entry.name.startswith("."):
                continue
            entries.append(entry)
            if len(entries) > max_seeds:
                raise SeedDirectoryError(
                    f"seed directory contains more than {max_seeds} files"
                )
    except OSError as error:
        raise SeedDirectoryError(f"cannot read seed directory: {seed_dir}") from error
    entries.sort(key=lambda entry: os.fsencode(entry.name))

    for entry in entries:
        try:
            mode = entry.lstat().st_mode
        except OSError as error:
            raise SeedDirectoryError(
                f"cannot inspect seed entry: {entry.name}"
            ) from error
        if not stat.S_ISREG(mode):
            raise SeedDirectoryError(
                "seed entries must be regular files, not links or directories: "
                f"{entry.name}"
            )
        if entry.suffix != ".json":
            raise SeedDirectoryError(f"seed file must end in .json: {entry.name}")

    return entries


def _read_seed(path: Path, max_bytes: int) -> bytes:
    """Read one regular file without following a last-component symbolic link."""
    if max_bytes < 1:
        raise ValueError("max_bytes must be positive")

    flags = os.O_RDONLY
    if hasattr(os, "O_NOFOLLOW"):
        flags |= os.O_NOFOLLOW
    descriptor: int | None = None
    try:
        descriptor = os.open(path, flags)
        file_stat = os.fstat(descriptor)
        if not stat.S_ISREG(file_stat.st_mode):
            raise SeedDirectoryError(f"seed is not a regular file: {path.name}")
        if file_stat.st_size == 0:
            raise SeedDirectoryError(f"seed is empty: {path.name}")
        if file_stat.st_size > max_bytes:
            raise SeedDirectoryError(
                f"seed exceeds the {max_bytes}-byte limit: {path.name}"
            )
        with os.fdopen(descriptor, "rb") as seed_file:
            descriptor = None
            contents = seed_file.read(max_bytes + 1)
    except SeedDirectoryError:
        raise
    except OSError as error:
        raise SeedDirectoryError(f"cannot read seed: {path.name}") from error
    finally:
        if descriptor is not None:
            os.close(descriptor)

    if not contents:
        raise SeedDirectoryError(f"seed is empty: {path.name}")
    if len(contents) > max_bytes:
        raise SeedDirectoryError(
            f"seed exceeds the {max_bytes}-byte limit: {path.name}"
        )
    if b"\0" in contents:
        raise SeedDirectoryError(f"seed contains a NUL byte: {path.name}")
    return contents


def load_seed_candidates(
    seed_dir: Path,
    *,
    max_seeds: int = DEFAULT_MAX_SEEDS,
    max_bytes: int = DEFAULT_MAX_SEED_BYTES,
    max_entries: int = DEFAULT_MAX_DIRECTORY_ENTRIES,
    require_nonempty: bool = False,
) -> list[bytes]:
    """Load distinct seeds from a bounded, flat ``*.json`` directory.

    Dotfiles are ignored so the distributed handout can retain an otherwise
    empty directory. Every visible entry must be a direct, regular ``.json``
    file. Duplicate byte strings are retained only once, in sorted filename
    order.
    """
    directory = Path(seed_dir)
    paths = _sorted_seed_paths(directory, max_seeds, max_entries)
    if require_nonempty and not paths:
        raise SeedDirectoryError(f"seed directory contains no .json files: {directory}")

    candidates: list[bytes] = []
    seen: set[bytes] = set()
    for path in paths:
        candidate = _read_seed(path, max_bytes)
        if candidate in seen:
            continue
        seen.add(candidate)
        candidates.append(candidate)

    if require_nonempty and not candidates:
        raise SeedDirectoryError(
            f"seed directory contains no distinct seeds: {directory}"
        )
    return candidates


def _build_validator(output_path: Path) -> None:
    """Build an uninstrumented oracle from the exact vendored JSON parser."""
    compiler_name = os.environ.get("CC", "clang")
    compiler = shutil.which(compiler_name)
    if compiler is None:
        raise JsonValidatorError(f"C compiler {compiler_name!r} was not found")

    command = [
        compiler,
        "-std=c11",
        "-O2",
        "-I",
        os.fspath(LAB_ROOT / "test/vendor/json-parser"),
        os.fspath(LAB_ROOT / "tools/json_seed_validator.c"),
        os.fspath(LAB_ROOT / "test/vendor/json-parser/json.c"),
        "-lm",
        "-o",
        os.fspath(output_path),
    ]
    try:
        completed = subprocess.run(
            command,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            timeout=60,
            check=False,
        )
    except (OSError, subprocess.TimeoutExpired) as error:
        raise JsonValidatorError("could not build the public JSON validator") from error
    if completed.returncode != 0:
        raise JsonValidatorError("could not build the public JSON validator")


def _validate_candidate(validator: Path, candidate: bytes, index: int) -> None:
    """Require one candidate to be accepted by the exact public parser."""
    try:
        completed = subprocess.run(
            [os.fspath(validator)],
            input=candidate,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            timeout=VALIDATOR_TIMEOUT_SECONDS,
            check=False,
        )
    except subprocess.TimeoutExpired as error:
        raise JsonValidatorError(
            f"JSON validation timed out for seed {index}"
        ) from error
    except OSError as error:
        raise JsonValidatorError("could not run the public JSON validator") from error

    if completed.returncode == 1:
        raise SeedDirectoryError(
            f"seed {index} is not accepted by the public JSON parser"
        )
    if completed.returncode != 0:
        raise JsonValidatorError("the public JSON validator failed")


def validate_seed_directory(seed_dir: Path) -> list[bytes]:
    """Load a nonempty corpus and validate every distinct seed exactly."""
    candidates = load_seed_candidates(seed_dir, require_nonempty=True)
    with tempfile.TemporaryDirectory(prefix="cis547-lab3-json-seeds-") as temporary:
        validator = Path(temporary) / "json_seed_validator"
        _build_validator(validator)
        for index, candidate in enumerate(candidates, start=1):
            _validate_candidate(validator, candidate, index)
    return candidates


def _parse_args(arguments: list[str] | None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate seeds for Lab 3's public JSON benchmark."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)
    validate = subparsers.add_parser("validate", help="validate a JSON seed directory")
    validate.add_argument("seed_dir", nargs="?", type=Path, default=DEFAULT_SEED_DIR)
    return parser.parse_args(arguments)


def main(arguments: list[str] | None = None) -> int:
    args = _parse_args(arguments)
    try:
        candidates = validate_seed_directory(args.seed_dir)
    except (JsonValidatorError, SeedDirectoryError, ValueError) as error:
        print(f"validate-json-seeds: {error}", file=sys.stderr)
        return 2

    suffix = "" if len(candidates) == 1 else "s"
    print(f"Validated {len(candidates)} distinct JSON seed{suffix}.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
