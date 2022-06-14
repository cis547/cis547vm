#! /usr/bin/env python3

import json

from contextlib import suppress
from typing import Dict, List, Tuple, Union
from pathlib import Path
from subprocess import run, PIPE
from sys import stderr

from tqdm import tqdm

from cbi.data_format import CBILog, CBILogEntry


def run_target(target: str, input: Union[str, bytes]) -> int:
    """
    Run the target program with input on its stdin.
    :param target: The target program to run.
    :param input: The input to pass to the target program.
    :return: The return code of the target program.
    """
    if isinstance(input, str):
        input = input.encode()
    process = run(
        [target],
        input=input,
        stdout=PIPE,
        stderr=PIPE,
    )

    # Debug information
    # print(
    #     f"ret={process.returncode}\n"
    #     f"out:\n{process.stdout}\n"
    #     f"err:\n{process.stderr}"
    # )
    return process.returncode


CBI_EXTENSION = ".cbi.jsonl"


def get_log_data_for_dir(
    target: str, input_dir: Path, expected_return_code: int = 0
) -> List[CBILog]:
    """
    Get the logs for the target program on the input files in the input_dir.

    :param target: The target program to run.
    :param input_dir: The directory containing the input files.
    :param expected_return_code: The expected return code of the target program.
    :return: A list of CBILogs, one for every file in input_dir.
    """
    log_file = Path(target).with_suffix(CBI_EXTENSION)

    # Clean up old file if necessary
    with suppress(FileNotFoundError):
        log_file.unlink()

    progress_bar = tqdm(
        [
            file
            for file in input_dir.glob("input*")
            if file.is_file() and len(file.suffixes) == 0
        ],
        desc=f"Processing {input_dir}",
        dynamic_ncols=True,
    )

    log_data: List[CBILog] = list()
    for file in progress_bar:
        #  Run the target program with file
        with open(file, "rb") as fp:
            return_code = run_target(target=target, input=fp.read())
            assert (
                return_code == expected_return_code
            ), f"return_code didn't match expected value: {expected_return_code}"

        # Move the log file to appropriate location.
        log_save_location = file.with_suffix(CBI_EXTENSION)
        log_file.rename(log_save_location)
        if not log_save_location.exists():
            log_data.append([])
        else:
            with log_save_location.open("r") as fp:
                # Parse all lines in the log file and add them to the log_data
                log_data.append(
                    [CBILogEntry(**json.loads(log_entry)) for log_entry in fp.readlines()]
                )
    return log_data


def get_logs(target: str, fuzz_dir: Path) -> Tuple[List[CBILog], List[CBILog]]:
    """
    Get all the logs for the target program.

    Runs the target program with each input file under fuzz_dir to generate .cbi.jsonl files.
    Parses the .cbi.jsonl files and returns two lists of CBILogs.

    :param target: The target program to run.
    :param fuzz_dir: The directory containing the fuzzer output.
    :return: Two lists of CBILogs,
        The first list contains logs for successful runs,
        and second list contains logs for failed runs.
    """
    success_dir = fuzz_dir / "success"
    failure_dir = fuzz_dir / "failure"

    success_dir.mkdir(parents=True, exist_ok=True)
    failure_dir.mkdir(parents=True, exist_ok=True)

    print("Collecting cbi logs...", file=stderr)
    success_logs = get_log_data_for_dir(
        target=target, input_dir=success_dir, expected_return_code=0
    )
    failure_logs = get_log_data_for_dir(
        target=target, input_dir=failure_dir, expected_return_code=1
    )

    return success_logs, failure_logs
