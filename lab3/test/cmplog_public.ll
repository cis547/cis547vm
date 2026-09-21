; Public fixed-input comparison logging check. The driver is not instrumented.
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"
@text = private constant [4 x i8] c"ABC\00"
@binary = private constant [3 x i8] c"\00C\FF"
declare i32 @strcmp(i8*, i8*)
declare i32 @strncmp(i8*, i8*, i64)
declare i32 @memcmp(i8*, i8*, i64)

define i32 @target(i8* %input, i64 %size) {
entry:
  %byte = load i8, i8* %input
  %comparison = icmp eq i8 %byte, -91
  br i1 %comparison, label %unreached, label %calls
unreached:
  %unused = icmp ne i64 %size, 123456789
  br label %calls
calls:
  %string = getelementptr i8, i8* %input, i64 1
  %text = getelementptr [4 x i8], [4 x i8]* @text, i64 0, i64 0
  %a = call i32 @strcmp(i8* %string, i8* %text)
  %b = call i32 @strncmp(i8* %string, i8* %text, i64 %size)
  %slice = getelementptr i8, i8* %input, i64 3
  %binary = getelementptr [3 x i8], [3 x i8]* @binary, i64 0, i64 0
  %c = call i32 @memcmp(i8* %slice, i8* %binary, i64 3)
  %ab = add i32 %a, %b
  %result = add i32 %ab, %c
  ret i32 %result
}
