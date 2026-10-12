# Lab 5 Writeup

Answer the questions in this file after completing Part 4, including the
CBI comparison runs. Provide short explanations to justify your answers,
and refer to your files in `llm/` (prompts, samples, transcripts, and final
answers) for concrete examples. Feel free to paste code/json/command examples in your answers using markdown formatting.

## Question 1

What LLM model and coding agent did you use?

Answer:

## Question 2

Why did we ask you to sample the LLM ten times? Should you increase the
number of samples or decrease? Should you sample at all?

Answer:

## Question 3

How did you decide on combining the multiple samples to one answer for the `combine` function implementation? Please explain your thought process.

Answer:

## Question 4

Compare the transcripts between the no tool usage and tool usage runs. What tools were most frequently used? Did the LLM get stuck on the task due to tool usage, and if so why/how/which tool? Compare the token usage and explain which approach is more efficient to use. When would you use tools, when would you not use tools, and why?

Answer:

## Question 5

Given your experience of using CBI with/without an LLM to find faults, which of the approaches do you prefer to use for fault localization? Do you prefer CBI over LLM, LLM over CBI, or using both? What are the trade-offs? Support your answer with concrete examples from your runs with and without the CBI report, for example whether the report changed the LLM's answers, turns, or token usage.

Answer:

## Question 6 (Extra Credit, Optional)

Create a program where the LLM incorrectly detected a fault line. The bug must still be a divide-by-zero bug. Include the model and coding agent used. You can take an existing test program and modify it yourself, or create your own. Why do you think the LLM failed to detect the fault line, and what else did you try?

Save your program in `llm/extra_credit/` and run your script on it, so its samples, transcripts, and final answer are included in your submission.

Program file:

Model and Coding Agent:

Answer:

## Question 7 (Extra Credit, Optional)

What did you think of the LLM portion? How long did it take? This is a new portion added to modernize the course with LLMs. Did you find this useful? Were you already familiar with working with LLMs thisi way? We appreciate any feedback.
