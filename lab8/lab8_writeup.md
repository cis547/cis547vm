# Lab 8 Writeup

Answer the questions in this file after running your LLM fact extraction
script on every test program. Provide short explanations to justify your
answers, and refer to your files in `llm/` (prompts, samples, transcripts,
and final facts) for concrete examples. Feel free to paste code/json/command
examples in your answers using markdown formatting.

## Question 1

What LLM did you use, and how did your script call it (for example, a provider's
API, a local model, or a coding agent's CLI)?

Answer:

## Question 2

Look at the programs in `test/llm` and the programs in `test`. What are the
differences, and why do you think we added the `test/llm` programs?

Answer:

## Question 3

How did you decide on combining the five samples of each relation into its
final facts in your `combine` function? Please explain your thought process.

Answer:

## Question 4

For each test program in `test/`, compare the alarms from your source-level
analysis on the LLM's facts with the alarms from your analysis in the previous
part. Where they differ, which facts caused the difference?

Answer:

## Question 5

Which relation did the LLM get wrong most often across your samples? Why do
you think that relation was harder than the others?

Answer:

## Question 6

Compare analyzing LLVM IR with analyzing source-level facts. Give an example of
a program that one representation can analyze precisely and the other cannot,
for example a division whose numerator is tainted but whose divisor is not, a
sanitizer call inside a larger expression, or several statements on one line.
Which representation would you trust more, and why?

Answer:

## Question 7 (Extra Credit, Optional)

Create a program where the LLM's facts cause your source-level analysis to miss
an exploitable divide-by-zero or report a false alarm. Include the LLM used. Why
do you think the LLM got the facts wrong, and what else did you try?

Save your program in `llm/extra_credit/` and run your script on it, so its
prompts, samples, transcripts, and final facts are included in your
submission.

Program file:

LLM:

Answer:

## Question 8 (Extra Credit, Optional)

What did you think of the LLM portion? How long did it take? Did you find this
useful? We appreciate any feedback.

Answer:
