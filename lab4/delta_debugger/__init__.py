from typing import Union
from subprocess import run, PIPE


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
