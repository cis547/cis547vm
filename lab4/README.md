
## Objective

In this lab, you will implement a delta debugger. This tool efficiently finds a 1-minimal crashing input given a larger input that causes a program to crash. You will then combine this delta debugger with a fuzzer (similar to the one developed in Lab 3) to minimize crashing inputs discovered by the fuzzer.

## Setup

The code for Lab 4 is located in the `/llvmlabs/reference/lab4/` directory. Throughout this document, we will refer to this as the `lab4` directory. Please open this directory in VSCode, following the instructions in the [Course VM document][course-vm].

This lab builds upon previous labs. You will compile the runtime library, `InstrumentPass` (for coverage and sanitization), and a fuzzer executable as part of the lab setup. Their implementations are identical to those in Lab 3.

### Step 1: Install the Delta Debugger Package

This lab utilizes Python for the delta debugger implementation, packaged as `delta_debugger`. To build and install this package, run the following command from the `lab4` directory:

```bash
make install
```

Unlike C++ projects, you generally won't need to re-run this command after making changes to your Python code. Once installed, you can use your delta debugger directly from the terminal via the `delta-debugger` command. This tool is designed to shrink a crashing input for a given program.

### Step 2: Find Crashing Inputs with a Fuzzer

Before using the delta debugger, you need an input that causes a program to crash. We will use a fuzzer for this purpose.

Similar to Lab 3, you'll first need to instrument the target program and set up output directories for the fuzzer's results. Navigate to the `lab4/test` directory and use the provided Makefile commands:

```bash
make sanity1          # Instrument and build sanity1
make fuzz-sanity1     # Run the fuzzer on sanity1
```

The `make fuzz-sanity1` command will automatically instrument, build, set up the output directory, and run the fuzzer for you.

### Step 3: Minimize Crashing Inputs using Delta Debugger

After running the fuzzer, crashing inputs will be stored under `lab4/test/fuzz_output_sanity1/failure/`. The directory structure will look like this:

```
fuzz_output_sanity1/
├── success/
├── randomSeed.txt
└── failure/          # Inputs that cause a crash.
    ├── input0
    ├── input1
    │    ...
    └── inputN
```

You can now use `delta-debugger` to minimize these crashing inputs. For example, to minimize `input1`:

```bash
delta-debugger ./sanity1 fuzz_output_sanity1/failure/input1
```

The last argument is the path to the crashing input you wish to minimize. The reduced input will be stored in a new file, for instance, `fuzz_output_sanity1/failure/input1.delta`.

**Important:** Before running another invocation of `delta-debugger` or the fuzzer, ensure you clean up the `fuzz_output_sanity1` directory to avoid conflicts:

```bash
rm -rf fuzz_output_sanity1 && mkdir fuzz_output_sanity1
```

## Lab Instructions

Your primary task is to implement the delta debugging logic within the `lab4/delta_debugger/delta.py` file. We have provided a template function, `delta_debug`, for you to complete. This function takes a target program and a crashing input, and it should return a 1-minimal input that still causes the target program to crash.

To perform delta debugging, you will repeatedly execute the target program with various input strings. A `run_target` function is provided to facilitate this:

```python
def run_target(target: str, input: Union[str, bytes]) -> int:
    """
    Run the target program with input on its stdin.
    :param target: The target program to run.
    :param input: The input to pass to the target program.
    :return: The return code of the target program (0 if no crash, non-zero otherwise).
    """
    ...
```

You will modify the `delta_debug` function to implement the delta debugging algorithm discussed in class. You might find it helpful to create a recursive helper function, for example, `_delta_debug`, which could take the target, an input, and a granularity parameter `n` to manage the search process.

## Example Input and Output

Your delta debugger should be capable of running on any executable that accepts input from stdin.

To run the delta debugger on a test program, use the following command format:

```bash
delta-debugger ./test crashing-input
```

The minimized result will be saved to a file named `crashing-input.delta`.

Consider this specific example where the string "abckdanmvelcbaghcajbtkzxmntplwqsrakstuvbxyz" causes `test3` to fail:

```bash
/lab4/test$ echo -n "abckdanmvelcbaghcajbtkzxmntplwqsrakstuvbxyz" > tmp
/lab4/test$ delta-debugger ./test3 tmp
/lab4/test$ cat tmp.delta
abckdanmvel
```

## Items to Submit

Once you have completed the lab, create a `submission.zip` file using the following command from the `lab4` directory:

```bash
make submit
```

This will generate `submission.zip` successfully. Please upload this file to Gradescope.

[course-vm]: {{ site.baseurl }}/resources/course-vm
