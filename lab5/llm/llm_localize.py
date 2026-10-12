#!/usr/bin/env python3
"""Localize divide-by-zero errors in a test program using an LLM.

Usage (from lab5/):
    python3 llm/llm_localize.py test/llm-localization/test6.c

Reads llm/prompt.txt and the given program, and samples the LLM
NUM_SAMPLES times. Saves the prompt to llm/samples/test6/prompt.txt and
each raw reply to llm/samples/test6/sampleNN.json and its session
transcript to llm/samples/test6/transcriptNN.log. Finally, writes the combined answer to llm/test6.localization.json.
"""
import argparse
import sys
from pathlib import Path

NUM_SAMPLES = 10
LLM_DIR = Path(__file__).resolve().parent


def build_prompt(template: str, file_name: str, source: str) -> str:
    """Return the full prompt for one program, built from your template.

    Args:
        template: The contents of prompt.txt.
        file_name: The program's file name, for example "test6.c".
        source: The program's source code.
    """
    # TODO: Build the prompt from your template and the program.
    raise NotImplementedError("build_prompt is not implemented")


def prompt_llm(prompt: str, transcript_path: Path) -> str:
    """Send the prompt to an LLM and return its final reply exactly as received.

    Also save everything your tool records about the session, such as tool
    calls and intermediate messages, to transcript_path. The transcript is for
    your writeup and is not graded.

    Args:
        prompt: The full prompt from build_prompt.
        transcript_path: Where to save the session transcript.
    """
    # TODO: Call the LLM of your choice, for example a command-line tool
    # through subprocess or a provider's Python SDK.
    raise NotImplementedError("prompt_llm is not implemented")


def combine(samples: list[str], file_name: str) -> str:
    """Combine the raw sample replies into one final answer in the required format.

    Args:
        samples: The LLM's raw replies, one per sample.
        file_name: The program's file name, for example "test6.c".
    """
    # TODO: Decide how to turn the samples into one final answer.
    raise NotImplementedError("combine is not implemented")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "program", type=Path,
        help="path to the test program, for example test/llm-localization/test6.c")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    source_path = args.program
    file_name = source_path.name
    program = source_path.stem
    if not source_path.is_file():
        print(f"{source_path} not found", file=sys.stderr)
        return 1

    template = (LLM_DIR / "prompt.txt").read_text()
    prompt = build_prompt(template, file_name, source_path.read_text())

    sample_dir = LLM_DIR / "samples" / program
    sample_dir.mkdir(parents=True, exist_ok=True)
    (sample_dir / "prompt.txt").write_text(prompt)
    samples = []
    for index in range(1, NUM_SAMPLES + 1):
        print(f"Sampling {file_name}: {index}/{NUM_SAMPLES}", file=sys.stderr)
        reply = prompt_llm(prompt, sample_dir / f"transcript{index:02}.log")
        (sample_dir / f"sample{index:02}.json").write_text(reply)
        samples.append(reply)

    final_path = LLM_DIR / f"{program}.localization.json"
    final_path.write_text(combine(samples, file_name))
    print(f"Wrote {final_path}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
