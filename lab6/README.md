# Lab 6: Dataflow Analysis

## 1. Objective

In this lab, you will build a static analyzer to automatically detect potential divide-by-zero errors in C programs at compile-time using LLVM. The lab is divided into two parts:

- Part 1: Implement the core logic for analyzing individual instructions (transfer functions and error checking)
- Part 2: Build a dataflow analysis framework using the chaotic iteration algorithm to analyze entire functions

By the end of this lab, your analyzer will be able to detect divide-by-zero errors like the one in this simple C program:

```c
// test01.c
int main() {
  int a = 0;
  int b = 1;
  int c = a + 1;
  int d = b / a;  // Divide by zero - your analyzer will catch this!
  if (c) {
    int a = 1;
  }
  return 0;
}
```

---

## 2. Conceptual Background

For a deeper exploration of these concepts, see [A Menagerie of Program Abstractions][Menagerie Link].

### Static Analysis & Abstract Interpretation

Static analysis analyzes software without executing it. Instead of tracking the concrete value of each variable (e.g., `a = 0`), which is often impossible to determine precisely, we approximate values using an **abstract domain**.

### The Abstract Domain

For this lab, our goal is to determine if a variable could be zero. We use the following abstract domain (defined in `include/Domain.h`):

- **`Zero`**: The variable is definitely zero (e.g., `int a = 0;`)
- **`NonZero`**: The variable is definitely not zero (e.g., `int b = 1;`)
- **`MaybeZero`**: The variable could be zero or non-zero (uncertain, e.g., user input)
- **`Uninit`**: The variable has not been initialized

These values form a lattice, where `MaybeZero` is the "top" (most general/conservative) and `Uninit` is the "bottom" (least informative).

### Transfer Functions

A **transfer function** models how a single instruction affects our abstract state. For example, in the instruction `c = a + 1;`:
- If `a` has abstract value `Domain::Zero`
- And the constant `1` is `Domain::NonZero`
- Then `Domain::add(Domain::Zero, Domain::NonZero)` returns `Domain::NonZero`
- So `c` gets the abstract value `Domain::NonZero`

### Dataflow Analysis & Chaotic Iteration

To analyze a whole function, we use dataflow analysis with a **chaotic iteration** algorithm:

1. Initialize a worklist with all instructions
2. While the worklist is not empty:
   - Pull an instruction `I` from the worklist
   - Flow-in: Join the `Out` states from all of `I`'s predecessors to compute its `In` state
   - Transfer: Apply the transfer function to produce a new `Out` state
   - Flow-out: If `I`'s `Out` state changed, add all successors to the worklist
3. When the worklist is empty, we've reached a fixed point

For each instruction, we maintain:
- `InMap[I]`: Abstract memory state before instruction `I`
- `OutMap[I]`: Abstract memory state after instruction `I`

An abstract memory state (`Memory`) is a map from LLVM variables (as strings) to abstract domain values.

---

## 3. Lab Structure and Workflow Overview

### Input Program Constraints

Your analysis only needs to handle a subset of C:
- All values are integers (ignore other types)
- Arithmetic operations: `+`, `-`, `*`, `/`
- Comparisons: `<`, `==`, etc.
- Control flow: if-statements and loops
- User input is introduced via functions where `isInput()` returns `True`
- Other instructions can be treated as no-ops

### Two-Part Implementation

Part 1: Instruction-Level Analysis
- Files to implement: `src/DivZeroAnalysis.cpp` and `src/Transfer.cpp`
- Implement `DivZeroAnalysis::check()` to detect potential divide-by-zero errors
- Implement transfer functions (`eval`) for binary operators, comparisons, and casts
- Build with the reference solution for Part 2 (use `-DUSE_REFERENCE=ON`)

Part 2: Function-Level Analysis
- Files to implement: `src/ChaoticIteration.cpp`
- Implement `join()` and `equal()` for merging and comparing memory states
- Implement `flowIn()` and `flowOut()` for dataflow propagation
- Implement `doAnalysis()` with the chaotic iteration worklist algorithm
- Build without the reference solution

### Utility Functions

The file `src/Utils.cpp` provides helpful functions:
- `variable(Value*)`: Converts an LLVM value to string representation
- `getOrExtract(Memory*, Value*)`: Gets abstract domain value for an LLVM value
- `printMemory()`, `printInstructionTransfer()`, `printMap()`: Debugging helpers

---

## 4. Part 1: Instruction-Level Analysis

Implement the logic for analyzing individual instructions. Use the pre-built reference solution for Part 2 to test your transfer functions.

### Memory Abstraction

Each `Instruction` has an associated abstract state:
- `InMap[I]`: Abstract memory before instruction `I`
- `OutMap[I]`: Abstract memory after instruction `I`

`Memory` is defined as `std::map<std::string, Domain *>`.

Example:

| ID   | Instruction                    | InMap (Before)      | OutMap (After)                  |
| ---- | ------------------------------ | ------------------- | ------------------------------- |
| `I1` | `%x = call i32 @input()`       | `{}`                | `{%x: MaybeZero}`               |
| `I2` | `%y = add i32 %x, 1`           | `{%x: MaybeZero}`   | `{%x: MaybeZero, %y: MaybeZero}`|

### Framework Entry Point

The `run()` method in `src/DivZeroAnalysis.cpp` is the entry point for your pass:

```cpp
PreservedAnalyses DivZeroAnalysis::run(Function &F, FunctionAnalysisManager &) {
  outs() << "Running " << getAnalysisName() << " on " << F.getName() << "\n";

  // Initialize InMap and OutMap for all instructions
  for (inst_iterator Iter = inst_begin(F), End = inst_end(F); Iter != End; ++Iter) {
    auto Inst = &(*Iter);
    InMap[Inst] = new Memory;
    OutMap[Inst] = new Memory;
  }

  // Run the chaotic iteration algorithm (Part 2)
  doAnalysis(F);

  // Check each instruction for potential divide-by-zero (Part 1)
  for (inst_iterator Iter = inst_begin(F), End = inst_end(F); Iter != End; ++Iter) {
    auto Inst = &(*Iter);
    if (check(Inst))
      ErrorInsts.insert(Inst);
  }
  // ...
}
```

### Implement `DivZeroAnalysis::check()`

File: `src/DivZeroAnalysis.cpp`

Implement the `check()` function to determine if a division instruction could result in divide-by-zero. Example: For `%d = sdiv i32 1, 0`, the divisor is `0` with abstract value `Domain::Zero`, so `check()` should return `true`.

### Implement Transfer Functions

File: `src/Transfer.cpp`

The main `transfer()` function dispatches to specific `eval()` functions based on instruction type. Implement:

1. `eval(BinaryOperator *BinOp, const Memory *In)` - Handle arithmetic operations (`+`, `-`, `*`, `/`). Get abstract domains of both operands, call the appropriate `Domain` method (e.g., `Domain::add()`, `Domain::sub()`), and return the resulting domain.

2. `eval(CmpInst *Cmp, const Memory *In)` - Handle comparison operations (`==`, `!=`, `<`, `>`). Get abstract domains of both operands, call `Domain::cmp()` with the appropriate predicate, and return the resulting domain.

3. `eval(CastInst *Cast, const Memory *In)` - Handle type casting operations. Get the abstract domain of the operand being cast. For integer casts, return the domain of the operand.

Note: `PHINode` instructions merge values from different control-flow paths. The implementation for `eval(PHINode*, ...)` is provided. Study how it joins domains from all incoming values to produce a single output domain. For a detailed explanation of PHI nodes, see [Appendix: Working with LLVM PHI Nodes](#10-appendix-working-with-llvm-phi-nodes).

### Build and Test Part 1


> ⚠️ **Important:** If you're on an x86-64 system, rename `reference/libHiddenAnalysis.x86-64.a` to `reference/libHiddenAnalysis.a`. Also, rename the original `libHiddenAnalysis.a` to avoid conflicts, since it was built for ARM64/AARCH64.

Build with the reference solution for Part 2:

```sh
lab6$ mkdir build && cd build
lab6/build$ cmake -DUSE_REFERENCE=ON ..
lab6/build$ make
```

This compiles your Part 1 implementation and links it with the reference solution for Part 2, generating `DivZeroPass.so`.

### Testing Workflow

Generate LLVM IR from the test program:

```sh
lab6/test$ clang -emit-llvm -S -fno-discard-value-names -Xclang -disable-O0-optnone -c -o test01.ll test01.c
```

This produces `test01.ll` with explicit memory operations (`alloca`, `store`, `load`):

```llvm
; test01.ll (excerpt)
define dso_local i32 @main() {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  ; 'a = 0'
  store i32 0, i32* %2, align 4
  ; 'b = 1'
  store i32 1, i32* %3, align 4
  ; 'c = a + 1'
  %7 = load i32, i32* %2, align 4
  %8 = add nsw i32 %7, 1
  store i32 %8, i32* %4, align 4
  ; 'd = b / a'
  %9 = load i32, i32* %3, align 4
  %10 = load i32, i32* %2, align 4
  %11 = sdiv i32 %9, %10
  store i32 %11, i32* %5, align 4
  ...
}
```

Run mem2reg optimization to promote local variables to SSA registers:

```sh
lab6/test$ opt -mem2reg -S test01.ll -o test01.opt.ll
```

This produces cleaner IR:

```llvm
; test01.opt.ll (excerpt)
define dso_local i32 @main() {
  %c = add nsw i32 0, 1
  %d = sdiv i32 1, 0
  %1 = icmp ne i32 %c, 0
  br i1 %1, label %2, label %4

2:
  br label %4

4:
  ret i32 0
}
```

Run your DivZero pass:

```sh
lab6/test$ opt -load-pass-plugin=../build/DivZeroPass.so -passes="DivZero" -disable-output test01.opt.ll > test01.out 2> test01.err
```

Output files:
- `test01.out`: Error report (stdout)
- `test01.err`: Debug output showing `InMap` and `OutMap` for each instruction (stderr)

See Section 6 for expected output.

### Debugging Part 1

If your output doesn't match expected results:

1. Check `test01.err` for the `InMap` and `OutMap` of each instruction
2. Verify transfer functions return the correct abstract domain
3. Use utility functions (`printMemory()`, `printInstructionTransfer()`) for debugging
4. Test incrementally with simple programs before moving to complex ones
5. Verify `Domain::add()`, `Domain::sub()`, etc. behave correctly

---

## 5. Part 2: Function-Level Analysis

Implement the chaotic iteration algorithm to analyze entire functions.

### Rebuild Without Reference Solution

Reconfigure CMake to build your own implementation:

```sh
lab6/build$ rm CMakeCache.txt
lab6/build$ cmake ..
lab6/build$ make
```

### Chaotic Iteration Algorithm

The `doAnalysis()` function in `src/ChaoticIteration.cpp` implements the worklist algorithm:

1. Initialize a worklist with all instructions in the function
2. Process instructions from the worklist until it's empty
3. For each instruction, perform flow-in, transfer, and flow-out operations
4. Reach a fixed point when no more changes occur

### Implement `join()`

File: `src/ChaoticIteration.cpp`

Implement `Memory* join(Memory *M1, Memory *M2)` to merge two abstract memory states. Create a new `Memory` object, join the domains for each variable in `M1` or `M2`, and include variables that appear in only one map.

Example:
```
M1 = {%x: Zero, %y: NonZero}
M2 = {%x: NonZero, %z: MaybeZero}
join(M1, M2) = {%x: MaybeZero, %y: NonZero, %z: MaybeZero}
```

### Implement `flowIn()`

File: `src/ChaoticIteration.cpp`

Implement `void flowIn(Instruction *I, Memory *In)` to compute the input state for instruction `I`. Get all predecessors of `I`, join their `OutMap` states, and store the result in the provided `In` memory. If `I` has no predecessors, `In` remains empty.

### Implement `equal()`

File: `src/ChaoticIteration.cpp`

Implement `bool equal(Memory *M1, Memory *M2)` to check if two memory states are identical. Return `true` if `M1` and `M2` have the same variables with the same domains.

### Implement `flowOut()`

File: `src/ChaoticIteration.cpp`

Implement `void flowOut(Instruction *I, Memory *Pre, Memory *Post, SetVector<Instruction *> &WorkSet)` to propagate changes to successor instructions. If `Post` differs from `Pre`, update `OutMap[I]` to `Post` and add all successors of `I` to the `WorkSet`.

### Implement `doAnalysis()`

File: `src/ChaoticIteration.cpp`

Implement `void doAnalysis(Function &F)` to perform the complete dataflow analysis:

1. Create a worklist
2. Initialize it with all instructions in the function `F`
3. While the worklist is not empty:
   - Pop an instruction `I` from the worklist
   - Compute the input state using `flowIn()`
   - Apply the transfer function
   - Propagate changes using `flowOut()`

### Control Flow Helpers

The framework provides helper functions to navigate the control-flow graph:

- `getPredecessors(Instruction *I)`: Returns a set of instructions that execute immediately before `I`
- `getSuccessors(Instruction *I)`: Returns a set of instructions that execute immediately after `I`

These handle the complexities of LLVM's CFG, including basic blocks and branch instructions.

### Testing Part 2

After implementing Part 2, rebuild and test:

```sh
lab6/build$ make
lab6/test$ opt -load-pass-plugin=../build/DivZeroPass.so -passes="DivZero" -disable-output test01.opt.ll > test01.out 2> test01.err
```

### Debugging Part 2

If the analysis doesn't converge or produces incorrect results:

1. Check the worklist - ensure instructions are added/removed correctly
2. Verify `join()` produces the correct least upper bound
3. Check `flowIn()` - verify that predecessor states are merged correctly
4. Verify `equal()` correctly identifies when states are identical
5. Check `flowOut()` - ensure successors are added to the worklist when states change
6. Add debug prints to show worklist contents and memory states at each iteration
7. Test on simple programs first (straight-line code), then add control flow

---

## 6. Expected Output

When your implementation is complete, running the analyzer on `test01.opt.ll` should produce the following results.

### Output File: `test01.out`

This file contains the error report:

```
Running DivZero on main
Potential Instructions by DivZero:
  %d = sdiv i32 1, 0
```

### Debug File: `test01.err`

This file shows the complete dataflow analysis results, displaying the `InMap` and `OutMap` for every instruction:

```
Dataflow Analysis Results:
Instruction: %c = add nsw i32 0, 1
In set:

Out set:
    [ %c       |-> NonZero   ]

Instruction: %d = sdiv i32 1, 0
In set:
    [ %c       |-> NonZero   ]
Out set:
    [ %c       |-> NonZero   ]
    [ %d       |-> Uninit    ]

Instruction: %1 = icmp ne i32 %c, 0
In set:
    [ %c       |-> NonZero   ]
    [ %d       |-> Uninit    ]
Out set:
    [ %1       |-> NonZero   ]
    [ %c       |-> NonZero   ]
    [ %d       |-> Uninit    ]

... (results for all other instructions)
```

### Understanding the Output

- In set: Shows the abstract state of all variables before the instruction executes
- Out set: Shows the abstract state of all variables after the instruction executes
- Domain values: `Zero`, `NonZero`, `MaybeZero`, or `Uninit`

For the divide instruction `%d = sdiv i32 1, 0`, the In set shows `%c` is `NonZero` (from the previous instruction). The divisor is `0`, which has domain `Zero`, so your `check()` function detects this and reports it as a potential divide-by-zero.

### Additional Test Cases

The `test/` directory contains additional test programs. Make sure your analyzer handles:
- Simple arithmetic
- Control flow (if statements, loops)
- PHI nodes (from merged control flow)
- User input (treated as `MaybeZero`)

---

## 7. Submission

Complete both parts and verify your implementation passes all test cases.

Create the submission file:

```sh
lab6$ make submit
```

This creates `submission.zip` containing your source files. Upload it to Gradescope.

---

## 8. Tips for Success

1. Start with Part 1: Get transfer functions working before attempting Part 2
2. Test incrementally: Use simple test cases first, then gradually increase complexity
3. Use debug output: The `test01.err` file shows `InMap` and `OutMap` for each instruction
4. Understand the framework: Read `Domain.h`, `Utils.cpp`, and the provided code carefully

### Useful LLVM Documentation
- Understanding instruction types (BinaryOperator, CmpInst, etc.)
- Navigating the CFG (basic blocks, successors, predecessors)
- Working with SSA form and PHI nodes

---

## 9. Appendix: Reference Links

### LLVM Documentation
- [LLVM `dyn_cast<>` Templates](http://releases.llvm.org/9.0.0/docs/ProgrammersManual.html#the-isa-cast-and-dyn-cast-templates)
- [LLVM `CmpInst` Class](https://llvm.org/doxygen/classllvm_1_1CmpInst.html)
- [LLVM `CastInst` Class](https://llvm.org/doxygen/classllvm_1_1CastInst.html)
- [LLVM `BinaryOperator` Class](https://llvm.org/doxygen/classllvm_1_1BinaryOperator.html)
- [LLVM `Instruction` Class](http://releases.llvm.org/9.0.0/docs/ProgrammersManual.html#the-instruction-class)
- [LLVM `SetVector` Container](https://llvm.org/doxygen/classllvm_1_1SetVector.html)

### Background Reading
- [A Menagerie of Program Abstractions][Menagerie Link]

[Menagerie Link]: {{ site.baseurl }}/resources/abstractions

---

## 10. Appendix: Working with LLVM PHI Nodes

For optimization purposes, compilers often implement their intermediate representation in static single assignment (SSA) form and LLVM IR is no different. In SSA form, a variable is assigned and updated at exactly one code point. If a variable in the source code has multiple assignments, these assignments are split into separate variables in the LLVM IR and then merged back together. We call this merge point a phi node.

To illustrate phi nodes, consider the following code:

```c
int f() {
  int y = input();
  int x = 0;
  if (y < 1) {
  # then
    x++;

  } else {
    x--;
  }
  # end
  return x;
}
```

The corresponding LLVM IR:

```llvm
entry:
  %call = call i32 (...) @input()
  %cmp = icmp slt i32 %call, 1
  br i1 %cmp, label %then, label %else
then:                      ; preds = %entry
  %inc = add nsw i32 0, 1  ; equates to x++ to the left
  br label %if.end
else:                      ; preds = %entry
  %dec = add nsw i32 0, -1 ; equates to x-- to the left
  br label %end
end:                       ; preds = %else, %then
  %x = phi i32 [%inc, %then], [%dec, %else]
  ret i32 %x
```

Depending on the value of `y`, we either take the left branch and execute `x++`, or the right branch and execute `x--`. In the corresponding LLVM IR, this update on `x` is split into two variables `%inc` and `%dec`. `%x` is assigned after the branch executes with the phi instruction; abstractly, `phi i32 [ %inc, %then ], [ %dec, %else ]` says assign `%inc` to `%x` if the `then` branch is taken, or `%dec` to `%x` if the `else` branch was taken.