## Objective

The objective of this lab is to use standard analysis tools and an LLM for static and
dynamic analysis on C programs to discover divide-by-zero errors and interpreting
their results to better understand various trade-offs between the techniques.

## Pre-Requisites

+ Watch the video lectures corresponding to the module on
“Introduction to Software Analysis”.
The lectures introduce various terminology used throughout this lab such as:
static and dynamic analysis, soundness, completeness, precision, F1, and more.

## Setup

### Step 1.

Set up the course development environment by following the instructions outlined
in [Course VM and Lab Instructions][course-vm].
The skeleton code for Lab 1 is located under `/lab1`.
We will refer to this top-level directory for Lab 1 simply as `lab1`
when describing file locations for the lab.

### Step 2.

Run the following command on your *local* machine under cis547vm folder to obtain
the latest changes to the lab:

```sh
./cis547vm$ git pull
```

### Step 3.

Throughout the labs, we will use `CMake`, a modern tool for
managing the build process.
If you’re unfamiliar with `CMake` we recommend reading the
[CMake tutorial][cmake-tutorial]
(especially pay attention to Step 1 and Step 2 in the tutorial).
Running `cmake` produces a `Makefile` that you might be more familiar with.
If you are not familiar with `Make`, read either the
[Makefile tutorial][makefile-tutorial]
or [Learn Make in Y minutes][learn-make-in-y-minutes] first,
and then peruse file `lab1/Makefile`.
Ensure that you are comfortable with using `Makefile` in this lab.

### Step 4.

Inspect the Makefile to see the commands used to run AFL++, IKOS and Gemini.

```sh
# Compile the program with AFL++
AFL_DONT_OPTIMIZE=1 AFL_USE_UBSAN=1 afl-clang-fast -fsanitize=integer-divide-by-zero c_programs/test1.c -o test1
# Run AFL++ for 30s on test1
AFL_NO_UI=1 timeout 30s afl-fuzz -i afl_input -o afl_output -m none test1
# Run IKOS on test1.c with interval and DBM abstraction respectively
ikos --opt none -a dbz -d interval c_programs/test1.c
ikos --opt none -a dbz -d dbm c_programs/test1.c
# Run Gemini on test1.c, asking it to be more conservative
gemini -p "analyze @test1.c and report any divide-by-zero errors (YES or NO). Prefer false positives over false negatives."
```

## Lab Instructions

In this lab, you will run three analysis tools on a suite of C programs,
study the tools’ results, and report your findings.

### Step 1.

Run the provided analysis tools, AFL++, IKOS, and Gemini on all C programs
located under the `lab1/c_programs` directory.
To do so, simply run the following command,
which first runs AFL++ with a timeout of 30 seconds per program,
and then runs IKOS with interval (int)
and difference bound matrix (dbm) abstractions,
followed by using the Gemini CLI with two different prompts.

```sh
/lab1$ make
```

On running the above command, you will see the output that looks
something like this:

```
AFL_DONT_OPTIMIZE=1 AFL_USE_UBSAN=1 afl-gcc c_programs/test1.c -o ...
AFL_NO_UI=1 timeout 30s afl-fuzz -i afl_input -o ...
Makefile:8: recipe for target 'results/afl_logs/test1/out.txt' failed
make: [results/afl_logs/test1/out.txt] Error 124 (ignored)
ikos --opt none -a dbz -d interval c_programs/test1.c ...
ikos --opt none -a dbz -d dbm c_programs/test1.c ...
# Sandboxing to avoid information leaking (Gemini will cheat off other tools)
cd /tmp/tmp.wHgzI1ZHNu && gemini -p ...
...
```

If you see an error from Gemini, that asks, `Please set an Auth method`,
make sure you followed the [Course VM and Lab Instructions][course-vm].

Ignore the error reported by Make above; it is normal because
AFL++ keeps running until it is forcibly terminated by the timeout command.
Feel free to experiment by changing the timeout which is set to 30 seconds.
You can remove `AFL_NO_UI=1` when running it yourself, to get a nice display.
We do not expect everyone to report the same solutions since AFL++ is non-deterministic anyway.

We encourage you to experiment with the prompts to Gemini.
See how the output differs when the prompt includes phrases like, "Explain your reasoning", "Provide a failing input", or "Give a score of how confident you are".

On running the make command, the following files and directories should be generated
under the `lab1/results` directory:

```
   ├── afl_logs/
   │   ├── test1/
   │   │   ├── out.txt
   │   │   ├── afl_output/
   │   │   └── test1
   │   ├── ... // similar for test2
   │   ...
   │
   ├── ikos_logs/
   │   ├── test1_dbm_out.txt
   │   ├── test1_int_out.txt
   │   ├── ... // similar for test2
   │   ...
   │
   └── gemini_logs/
       ├── test1_overapprox_out.txt
       ├── test1_underapprox_out.txt
       ├── ... // similar for test2
       ...
```

### Step 2.

Determine the ground truth (right vs. wrong) of the C programs with respect to
division-by-zero errors.
In particular, for each division instruction in each program, determine by
inspecting the program whether it can result in a division-by-zero error on
some program input.
Write your answers in file `lab1/answers.txt` in the “ground truth” column
of the table for each test.
We provide the ground truth for the last programs already.

### Step 3.

Study the output of AFL++, IKOS, and Gemini, and determine if they accept or reject each program.
Fill in your answers in file `lab1/answers.txt` in the corresponding columns of
the table for each test program.

The crashing inputs discovered by AFL++ are stored in separate files under
directories `lab1/results/afl_logs/<test-name>/afl_output/crashes/`.
The files have idiosyncratic names of the form
`id:000000,sig:08,src:000000,op:arith8,pos:2,val:-8`.[^1]
It is the *contents* of these files that AFL++ used as the input
to the test program when it encountered a crash.

### Step 4.

Using your entries from Steps 2 and 3, calculate the
Precision, Recall, and F1 Score of each column.
Enter them in the corresponding rows in `lab1/answers.txt`

### Step 5.

Answer the questions in `answers.txt` with the help of the table you filled in.

## Submission

Once you are done with the lab, you can create a `submission.zip` file by using the following command:

```sh
lab1$ make submit
...
submission.zip created successfully.
```

Then upload the `submission.zip` file to Gradescope.

[^1]: The file name encodes various things such as the ID of the crashing input, the crashing signal, the non-crashing seed input from which this crashing input was produced -- which in our case is always the file lab1/afl_input/seed.txt, and the operations by which the non-crashing seed input was transformed into this crashing input.

[course-vm]: https://cis547.github.io/resources/course-vm
[cmake-tutorial]: https://cmake.org/cmake/help/latest/guide/tutorial/index.html
[makefile-tutorial]: https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html
[learn-make-in-y-minutes]: https://learnxinyminutes.com/docs/make/
