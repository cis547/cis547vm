
### Objective 

In this lab, you will implement a constraint-based analysis to detect exploitable divide-by-zero bugs.
A bug is exploitable if hackers can control inputs or environments, thereby triggering unintended behaviors (e.g., denial-of-service) through the bug.
For example, [a recently reported divide-by-zero bug][bug] in the Linux kernel can be exploitable and crash the system; another example was the [Log4Shell][log4shell] incident.
You will design a static analysis that detects such bugs by combining reaching definition analysis and taint analysis using a datalog engine, souffle. Afterwards, you will use LLMs to assist with extracting facts, combined with the static analysis. 

### Setup

The skeleton code for Lab8 is located under `cis547/lab8/`.
We will frequently refer to the top level directory for Lab 8 as `lab8` when describing file locations for the lab. Open the `lab8` directory in VSCode following the Instructions from [Course VM document][course-vm]

The following commands set up the lab, using the [Cmake][CMake Ref]/[Makefile][Make Ref] pattern seen before.

```sh
/lab8$ mkdir build && cd build
/lab8/build$ cmake ..
/lab8/build$ make
```

The above command will generate an executable file 'constraint' in build directory that extracts facts about the program that will be used by `src/analysis.dl`
to check if the input program has an exploitable divide-by-zero bug:

```sh
/lab8$ cd ./test
/lab8/test$ clang -emit-llvm -S -fno-discard-value-names -c simple0.c
/lab8/test$ mkdir -p simple0/input simple0/output
/lab8/test$ ../build/constraint simple0.ll ./simple0/input
/lab8/test$ souffle -F ./simple0/input -D ./simple0/output ../src/analysis.d
```

After completing the lab implementation, you should see the instruction that could potentially cause a divide-by-zero in `./simple0/output/alarm.csv`. 

### Lab Instructions

In this lab, you will design a reaching definition analysis and taint analysis using [datalog][souffle].

The main tasks are to design the analysis in the form of logical rules as a Datalog program, and implement a function that extracts logical relations form a test program in the form of Datalog facts for each LLVM instruction.

We will then use the datalog rules with the facts you extracted from each instruction in a program to find any *exploitable* divide-by-zero errors.

In short, the lab consists of the following tasks:

1. Write Datalog rules in `analysis.dl` for taint analysis.
2. Implement the `extractContraints` function in `Extractor.cpp` that extracts Datalog facts from LLVM IR `Instruction` and dumps them to appropriate `.facts` files.
   Detailed instructions provided in comments.

#### Relations for Datalog Analysis

The skeleton code in `Extractor.cpp` provides the definitions of various functions that will help you dump the necessary Datalog relations declared in `src/analysis.dl` file.

Here we will explain what each of the datalog relation mean.

The relations for def and use of variables are as follows:

- `def(var, inst)`: Variable `var` is defined at instruction `inst`.
- `use(var, inst)`: Variable `var` is used in instruction `inst`.

The relations for the reaching definition analysis are as follows: 

- `kill(curr_inst, old_inst)`: Instruction `curr_inst` kills definition at instruction `old_inst`.
- `next(curr_inst, next_inst)`: Instruction `next_inst` is an immediate successor of instruction `curr_inst`.
- `in(inst, def_inst)`: Definition at defining instruction `def_inst` may reach the program point just before instruction `inst`.
- `out(inst, def_inst)`: Definition at defining instruction `def_inst` may reach the program point just after instruction `inst`.

Note that the `kill` relation can be derived by using the `def` relation by writing a Datalog rule.

The relations for the taint analysis are as follows: 

- `taint(inst)` : There exists a function call at intruction `inst` that reads a tainted input.
- `edge(from, to)`: There exists an immediate data-flow from instruction `from` to instruction `to`.
- `Path(from, to)`: There exists a transitive tainted data-flow from instruction `from` to instruction `to`.
- `sanitizer(inst)` : There exists a function call at intruction `inst` that sanitizes a tainted input.
- `div(denom, inst)` : There exists a division operation at instruction inst whose divisor is variable denom.
- `alarm(inst)` : There exists a potential exploitable divide-by-zero error at instruction inst.


Assume that input programs may contain function calls to `tainted_input` and `sanitizer` that read and sanitize a tainted input, respectively.
The final output relation for potential bug reports is `alarm`.

You will use these relations to build rules for the definition analysis and taint analysis in `analysis.dl`.

#### Encoding LLVM Instruction in Datalog

Recall that, in LLVM IR, values and instructions are interchangable.
Therefore, all variables X, Y, and Z are an instance of LLVM’s `Value` class. 

Assume that input C programs do not have pointer variables.
Therefore, we abuse pointer variables in LLVM IR as their dereference expressions.
Consider the following simplified LLVM program from a simple C program `int x = 0; int y = x;`:

```llvm
x = alloca i32          ; I0
y = alloca i32          ; I1
store i32 0, i32* x     ; I2
a = load i32, i32* x    ; I3
store i32 a, i32* y     ; I4
```

We ignore alloca instructions and consider that each store instruction defines the second argument.

In the case of the above example, you should have `addDef(I0,I2)`, because `I0` corresponds to `x` in LLVM IR and `I2` defines the variable `x`.
Likewise, consider each load instruction uses the argument.
In the example, you should have `addUse(I0,I3)` and `addDef(I3,I3)` because load instructions define a non-pointer variable which is represented as the instruction itself in LLVM.
Finally, you should have `addUse(I3,I4)` and `addDef(I1,I4)` for instruction `I4`.

#### Defining Datalog Rules

You will write your Datalog rules for taint analysis in `src/analysis.dl` using the relations above.

Please refer to the souffle's [documentation][souffle] for the datalog language and its syntax.

Here's a quick example to help you get started.

Consider an example Datalog rule:

```dl
A(X, Y) :- B(X, Z), C(Z, Y).
```

It adds a rule which says that there is a relation `A` between `X` and `Y` if,
there is a relation `B` between `X` and some `Z` and there is also a
relation `C` between that `Z` and `Y`.

#### Extracting Datalog Facts

You will need to implement the function `extractConstraints` in
`Extractor.cpp` to extract Datalog facts for each LLVM instruction.
The skeleton code provides a couple of auxiliary functions in `lab8/src/Extract.cpp` and `lab8/src/Utils.cpp` help you with this task:

- `void addX(const InstMapTy &InstMap, ...)`
- - `X` denotes the name of a relation. These functions add a fact to the facts file for `X`.
- - It takes `InstMap` that encodes each LLVM instruction as an integer. This map is initialized in the `main` function.
- `vector<Instruction*> getPredecessors(Instruction *I)`
- - Returns a set of predecessors of a given LLVM instruction `I`.
- `bool isTaintedInput(CallInst *CI)`
- - Checks whether a given LLVM call instruction `CI` reads a tainted input or not.
- `bool isSanitizer(CallInst *CI)`
- - Checks whether a given LLVM call instruction `CI` sanitizes a tainted input or not.

#### Miscellaneous

For debugging, after you run `constraint` you can inspect the `./test/simple0/input` for files containing the extracted relations.

### Format of Input Programs

Input programs in this lab are assumed to have only sub-features of the C language as follows:

- All values are integers (i.e., no floating points, pointers, structures, enums, arrays, etc). You can ignore other types of values.
- Assume that there is no function call to a function with a void return type.
- You must handle the function calls to `tainted_input` and `sanitizer` in a special way which represents their actions as described previously.

### Example Input and Output

Your analyzer should run on LLVM IR. For example:

```sh
/lab8$ cd ./test
/lab8/test$ make loop0
```

If the input program has exploitable divide-by-zero errors, then you should see entries in `loop0/output/alarm.csv`.

```llvm
%div
```

### Using LLMs to Extract Facts

So far, `constraint` has extracted facts from LLVM IR, where every load, store, and temporary value has its own name. In this part, you will instead use an LLM to read a test program's C source code and identify the facts, at the level of source code statements. You will then run a source-level version of your taint analysis on the LLM's facts and compare its alarms with your analysis from the previous part.

This part consists of the following tasks:

1. Write Datalog rules in `src/llm_analysis.dl` for source-level taint analysis.
2. Write one prompt per relation in `llm/prompts/`, and implement the functions in `llm/llm_extract.py` that prompt an LLM, sample it five times per relation, combine the replies, and write the facts.
3. Run your script on every test program in `test/` and `test/llm/`, and answer the questions in `lab8_writeup.md`.

#### Source-Level Facts

The source-level facts use the same relation names as before, but name statements by their line number and variables by their names in the C source:

- `def(var, stmt)`: Statement `stmt` assigns variable `var`.
- `use(var, stmt)`: Statement `stmt` reads variable `var`.
- `next(stmt, next_stmt)`: Statement `next_stmt` can run immediately after statement `stmt`.
- `taint(stmt)`: Statement `stmt` assigns the result of a call that returns input from outside the program, such as a call to `tainted_input`, reading standard input, or parsing a command-line argument or an environment variable.
- `sanitizer(stmt)`: Statement `stmt` assigns a value computed from a variable that can never be zero, whatever the variable holds, such as the result of a call to `sanitizer`.
- `div(var, stmt)`: Statement `stmt` divides by variable `var`.

Facts must follow these conventions:

- A statement is named `L` followed by its line number. For example, `int y = x;` on line 5 is `L5`.
- The parameters of a function are defined by the statement on the line of the function's header. For example, `int main(int argc, char** argv) {` on line 3 gives `def(argc, L3)` and `def(argv, L3)`.
- A declaration without a value, such as `int x;`, is not a statement and has no facts.
- The condition of an `if` or `while` is a statement that uses the variables in the condition. For example, `while (argc > 1) {` on line 7 gives `use(argc, L7)`.
- `next` follows the program's control flow as written. A condition leads to the first statement of each branch, the last statement of a loop body leads back to the loop's condition, and a condition also leads to the first statement after its `if` or loop. The last statement of a then branch leads to the first statement after the whole `if`, skipping the `else` branch. Conditions are not evaluated, so both branches of `if (1)` are included.
- Braces, `else`, and blank lines are not statements. A `return` statement, such as `return 0;`, is a statement. It has no outgoing `next` fact, because nothing runs after it.
- A function's header line is always its first statement, even when the function has no parameters.
- A variable that is assigned but never read still has a `def` fact.
- A statement such as `argc--` both uses and defines its variable.
- Reading an element of an array, such as `buf[i]`, uses the array's variable, `buf`, and the index variable, `i`.
- Reading `argc` or `argv` without a call that parses it, such as `int n = argc;`, is not a taint statement.
- Calls to other functions, such as `not_sanitizer`, and expressions that can still be zero, such as `x - 1`, are not sanitizers. Assigning a constant, such as `y = 100;`, is not a sanitizer either.
- Only `/` is a division for `div`; `%` is not, and dividing by a constant, such as `x / 2`, has no `div` fact.
- Every input relation must have a `.facts` file, even when it has no facts.

For example, consider `test/simple1.c`:

```c
3  int main(int argc, char** argv) {
4    int x = tainted_input();
5    int y = sanitizer(x);
6    int z = 4 / y;  // safe
7
8    y = not_sanitizer(x);
9    z = 4 / y;  // alarm
10   return 0;
11 }
```

Its facts are:

| Relation | Facts |
| :------- | :---- |
| `def` | `argc L3`, `argv L3`, `x L4`, `y L5`, `z L6`, `y L8`, `z L9` |
| `use` | `x L5`, `y L6`, `x L8`, `y L9` |
| `next` | `L3 L4`, `L4 L5`, `L5 L6`, `L6 L8`, `L8 L9`, `L9 L10` |
| `taint` | `L4` |
| `sanitizer` | `L5` |
| `div` | `y L6`, `y L9` |

As in the `.facts` files that `constraint` writes, the columns of each fact are separated by a tab, and each fact is on its own line.

#### Source-Level Taint Analysis

`src/llm_analysis.dl` declares the source-level relations above and provides the reaching definition analysis, which is the same as in `src/analysis.dl`. You will write the rules for the same relations as in `src/analysis.dl`, with statements in place of instructions:

- `edge(from, to)`: There exists an immediate data-flow from statement `from` to statement `to`.
- `path(from, to)`: There exists a transitive tainted data-flow from statement `from` to statement `to`.
- `alarm(stmt)`: There exists a potential exploitable divide-by-zero error at statement `stmt`.

Your rules for `edge` and `path` carry over from `src/analysis.dl`. Your rule for `alarm` needs to change: in LLVM IR, `div(denom, inst)` names the instruction that holds the divisor, which `path` can reach. At the source level, `div(var, stmt)` names the divisor by its variable, which is not a statement, so `path` cannot reach it.

For `test/simple1.c`, `alarm.csv` should contain only `L9`: the division on line 6 uses the value of `y` assigned by `sanitizer` on line 5, while the division on line 9 uses the value assigned by `not_sanitizer` on line 8.

#### LLM Fact Extraction Script

You will complete the functions in `llm/llm_extract.py` that have the comment `# TODO` in the function body. For one test program, the script prompts an LLM once per relation: it samples each relation's prompt five times, and combines that relation's five replies into its final facts. You decide how to combine the samples.

You can use any LLM, for example a provider's API or Python SDK, a model running locally, or a command-line coding agent such as `claude -p`. Pick one that you can call from a script many times. Please execute your script in the docker container.

Write one prompt per relation in `llm/prompts/`: `def.txt`, `use.txt`, `next.txt`, `taint.txt`, `sanitizer.txt`, and `div.txt`. The script reads each one and uses it for every program. Each prompt is sent on its own, so each must include everything the LLM needs for its relation, such as how to name statements and variables. Run the script from the `lab8` project root with the path to a test program:

```sh
lab8$ python3 llm/llm_extract.py test/simple1.c
```

When running the script, it saves the following for each program and relation:

- The generated prompt: `llm/samples/<program>/<relation>/prompt.txt`
- Each sample's reply, exactly as the LLM returned it: `llm/samples/<program>/<relation>/sample01.json` through `sample05.json`
- Each sample's session transcript: `llm/samples/<program>/<relation>/transcript01.log` through `transcript05.log`
- The final facts, combined from the relation's 5 samples: `llm/facts/<program>/<relation>.facts`

Each program takes 30 LLM calls: 6 relations with 5 samples each, or 300 calls for all ten test programs.

Save everything your LLM returns about the session, such as the full API response or a CLI's JSON or verbose output (for example, `claude -p --output-format stream-json --verbose`). The transcript format is up to you; it is not graded.

Then run your source-level analysis on the LLM's facts:

```sh
lab8/test$ make simple1-llm
lab8/test$ cat simple1/llm-output/alarm.csv
L9
```

#### Reply Format

Each reply must be a JSON object with exactly two keys, `file` and the name of the prompt's relation, and nothing else in the file. For example, a reply to `def.txt` and a reply to `taint.txt` for `test/simple1.c`:

```json
{"file": "simple1.c", "def": [["argc", "L3"], ["argv", "L3"], ["x", "L4"]]}
```

```json
{"file": "simple1.c", "taint": ["L4"]}
```

- `file` is the program's file name, for example `"simple1.c"`.
- `def`, `use`, `next`, and `div` list facts as two-element lists of strings, in the column order of the relation.
- `taint` and `sanitizer` list statement names as strings.
- A relation with no facts is an empty list.

A reply with Markdown code fences, explanations, or extra keys does not follow the format.

#### Rules

- Using an LLM is required. Save every reply exactly as the LLM returned it. Do not edit replies or facts by hand.
- The LLM may access only the program it is extracting facts from. It may not use tools, and it may not see the `.ll` file, the facts written by `constraint`, or any other file.
- The LLM cannot use persisted memory between samples and test programs.
- Use the same six prompts in `llm/prompts/` for all test programs.
- Every fact in your final facts must appear in at least one LLM reply to its relation's prompt that follows the format. You choose how to combine the replies, but the final facts must come from them.

Run your script on every test program in `test/`: `simple0`, `simple1`, `branch0`, `branch1`, `branch2`, `loop0`, and `loop1`. Also run it on the programs in `test/llm/`: `real0`, `real1`, and `real2`. They are only for this part. `constraint` does not analyze them.

#### Hints

- Each prompt should explain the conventions above that matter for its relation. The LLM does not know them unless you include them, and the source-level facts are graded against facts that follow them.
- Read the replies in your samples, not only the final facts. Because each relation has its own samples, you can improve one prompt at a time. They show which facts the LLM gets wrong, and how often.
- To find out why an alarm differs between your two analyses, compare `<program>/llm-output/path.csv` and `edge.csv` with the program's source, then check the facts in `llm/facts/<program>/` that they depend on.
- Each sample should depend only on your prompt and the program. Many LLM tools load instruction files (such as `CLAUDE.md`, `GEMINI.md`, or `AGENTS.md`) or save memories between sessions. Run each sample in a fresh, empty directory, and check your home directory for global instruction files.

#### Short Answer Questions

Please answer the questions in `lab8_writeup.md`.

1. What LLM did you use, and how did your script call it (for example, a provider's API, a local model, or a coding agent's CLI)?
2. Look at the programs in `test/llm` and the programs in `test`. What are the differences, and why do you think we added the `test/llm` programs?
3. How did you decide on combining the five samples of each relation into its final facts in your `combine` function? Please explain your thought process.
4. For each test program in `test/`, compare the alarms from your source-level analysis on the LLM's facts with the alarms from your analysis in the previous part. Where they differ, which facts caused the difference?
5. Which relation did the LLM get wrong most often across your samples? Why do you think that relation was harder than the others?
6. Compare analyzing LLVM IR with analyzing source-level facts. Give an example of a program that one representation can analyze precisely and the other cannot, for example a division whose numerator is tainted but whose divisor is not, a sanitizer call inside a larger expression, or several statements on one line. Which representation would you trust more, and why?
7. Extra Credit/Optional (+20 points) - Create a program where the LLM's facts cause your source-level analysis to miss an exploitable divide-by-zero or report a false alarm. Include the LLM used. Why do you think the LLM got the facts wrong, and what else did you try? Save your program in `llm/extra_credit/` and run your script on it, so its prompts, samples, transcripts, and final facts are included in your submission.
8. Extra Credit/Optional (+1 point) - What did you think of the LLM portion? How long did it take? Did you find this useful? We appreciate any feedback.

### Submission


Your submission must include the following files in `llm/`, for every program and relation:

- The generated prompt: `llm/samples/<program>/<relation>/prompt.txt`
- All five replies: `llm/samples/<program>/<relation>/sample01.json` through `sample05.json`. The autograder checks that every final fact appears in at least one of them.
- All five transcripts: `llm/samples/<program>/<relation>/transcript01.log` through `transcript05.log`
- The final facts: `llm/facts/<program>/<relation>.facts`

`make submit` packs `llm/` into a single `llm.tar.gz` inside `submission.zip`, because of Gradescope's file upload limit.

Once you are done with the lab, you can create a `submission.zip` file by using the following command:
```sh
lab8$ make submit
...
submission.zip created successfully.
```

[CMake Ref]: https://en.wikipedia.org/wiki/CMake
[Make Ref]: https://www.gnu.org/software/make/manual/html_node/
[course-vm]: {{ site.baseurl }}/resources/course-vm
[bug]: https://www.cvedetails.com/cve/CVE-2019-14284/
[log4shell]: https://en.wikipedia.org/wiki/Log4Shell
[souffle]: https://souffle-lang.github.io/simple