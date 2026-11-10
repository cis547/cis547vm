#!/usr/bin/env python3
"""Extract source-level Datalog facts from a test program using an LLM.

Usage (from lab8/):
    python3 llm/llm_extract.py test/simple1.c

For each relation, such as def, reads llm/prompts/def.txt and the given
program, and samples the LLM NUM_SAMPLES times. Saves the prompt to
llm/samples/simple1/def/prompt.txt, each raw reply to
llm/samples/simple1/def/sampleNN.json, and each session transcript to
llm/samples/simple1/def/transcriptNN.log. Finally, writes the combined facts
to llm/facts/simple1/, one <relation>.facts file per relation.
"""
import argparse
import sys
from pathlib import Path

NUM_SAMPLES = 5
LLM_DIR = Path(__file__).resolve().parent
PROMPT_DIR = LLM_DIR / "prompts"
# Each input relation of src/llm_analysis.dl and its number of columns.
RELATIONS = {"def": 2, "use": 2, "next": 2, "taint": 1, "sanitizer": 1, "div": 2}


def build_prompt(template: str, file_name: str, source: str) -> str:
    """Return the full prompt for one program, built from your template.

    Args:
        template: The contents of one relation's prompt, such as prompts/def.txt.
        file_name: The program's file name, for example "simple1.c".
        source: The program's source code.
    """
    # TODO: Build the prompt from your template and the program.
    raise NotImplementedError("build_prompt is not implemented")


def prompt_llm(prompt: str, transcript_path: Path) -> str:
    """Send the prompt to an LLM and return its final reply exactly as received.

    Also save everything your tool records about the session, such as
    intermediate messages, to transcript_path. The transcript is for your
    writeup and is not graded.

    Args:
        prompt: The full prompt from build_prompt.
        transcript_path: Where to save the session transcript.
    """
    # TODO: Call the LLM of your choice, for example a command-line tool
    # through subprocess or a provider's Python SDK.
    raise NotImplementedError("prompt_llm is not implemented")


def combine(samples: list[str], file_name: str, relation: str) -> list[tuple[str, ...]]:
    """Combine one relation's raw sample replies into its final facts.

    Args:
        samples: The LLM's raw replies to the relation's prompt, one per sample.
        file_name: The program's file name, for example "simple1.c".
        relation: The relation's name, for example "def".

    Returns:
        The relation's facts. Each fact is a tuple with one string per
        column, for example [("x", "L4"), ("y", "L5")] for def, or [("L4",)]
        for taint.
    """
    # TODO: Decide how to turn the samples into one final set of facts.
    raise NotImplementedError("combine is not implemented")


def write_facts(facts: dict[str, list[tuple[str, ...]]], out_dir: Path) -> None:
    """Write each relation's facts to out_dir/<relation>.facts for Souffle.

    Args:
        facts: The combined facts from combine.
        out_dir: The directory to write the .facts files to.
    """
    # TODO: Write one .facts file per relation in RELATIONS, in the format
    # Souffle reads with .input.
    raise NotImplementedError("write_facts is not implemented")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "program", type=Path,
        help="path to the test program, for example test/simple1.c")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    source_path = args.program
    file_name = source_path.name
    program = source_path.stem
    if not source_path.is_file():
        print(f"{source_path} not found", file=sys.stderr)
        return 1

    source = source_path.read_text()
    facts = {}
    for relation in RELATIONS:
        template = (PROMPT_DIR / f"{relation}.txt").read_text()
        prompt = build_prompt(template, file_name, source)

        sample_dir = LLM_DIR / "samples" / program / relation
        sample_dir.mkdir(parents=True, exist_ok=True)
        (sample_dir / "prompt.txt").write_text(prompt)
        samples = []
        for index in range(1, NUM_SAMPLES + 1):
            print(f"Sampling {relation} for {file_name}: {index}/{NUM_SAMPLES}",
                  file=sys.stderr)
            reply = prompt_llm(prompt, sample_dir / f"transcript{index:02}.log")
            (sample_dir / f"sample{index:02}.json").write_text(reply)
            samples.append(reply)
        facts[relation] = combine(samples, file_name, relation)

    facts_dir = LLM_DIR / "facts" / program
    facts_dir.mkdir(parents=True, exist_ok=True)
    write_facts(facts, facts_dir)
    print(f"Wrote {facts_dir}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
