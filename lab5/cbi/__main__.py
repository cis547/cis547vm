#! /usr/bin/env python3

import json
import sys

from dataclasses import asdict
from pathlib import Path

from cbi.cbi import cbi
from cbi.utils import get_logs


def main() -> int:
    """
    Usage: cbi [target] [fuzzer-output-dir]
    """
    if len(sys.argv) < 3:
        print(
            f"Usage: cbi [target] [fuzzer-output-dir]",
            file=sys.stderr,
        )
        return 1
    target, fuzz_output_dir = sys.argv[1:]

    if not Path(target).exists():
        print(f"{target} not found", file=sys.stderr)
        return 1
    if not Path(fuzz_output_dir).exists():
        print(f"{fuzz_output_dir} not found", file=sys.stderr)
        return 1

    # Generate the cbi logs
    success_logs, failure_logs = get_logs(target=target, fuzz_dir=Path(fuzz_output_dir))
    # Analyze the cbi logs and generate the report
    report = cbi(success_logs=success_logs, failure_logs=failure_logs)
    # Visualize the report
    print(report)
    # Save the report to a file
    with open(f"{target}.report.json", "w") as fp:
        json.dump(asdict(report), fp, indent=4)
    return 0


if __name__ == "__main__":
    """
    Usage: cbi [target] [fuzzer-output-dir]
    """
    sys.exit(main())
