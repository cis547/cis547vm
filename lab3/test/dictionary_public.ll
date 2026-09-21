; Representative public fixture for the standalone BuildDictionary pipeline.
; The explicit data layout makes integer serialization deterministic.
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@global_bytes = private constant [5 x i8] c"A\00B\FF\00"
@strcmp_value = private constant [8 x i8] c"PUBLIC!\00"
@prefix_value = private constant [7 x i8] c"PREFIX\00"
@binary_value = private constant [5 x i8] c"X\00Q\FFY"
@needle_value = private constant [8 x i8] c"XNEEDLE\00"

declare i32 @strcmp(i8*, i8*)
declare i32 @strncmp(i8*, i8*, i64)
declare i32 @memcmp(i8*, i8*, i64)
declare i8* @strstr(i8*, i8*)

define i32 @dictionary_public(i8* %buffer, i16 %number, i32 %selector) {
entry:
  %local = alloca [32 x i8], align 1
  %local.byte = getelementptr inbounds [32 x i8], [32 x i8]* %local, i64 0, i64 5
  %byte = load i8, i8* %local.byte, align 1
  %byte.matches = icmp eq i8 %byte, 90
  %number.below = icmp ult i16 %number, 258

  %strcmp.input = getelementptr inbounds i8, i8* %buffer, i64 2
  %strcmp.constant = getelementptr inbounds [8 x i8], [8 x i8]* @strcmp_value, i64 0, i64 0
  %strcmp.result = call i32 @strcmp(i8* %strcmp.input, i8* %strcmp.constant)

  %strncmp.input = getelementptr inbounds i8, i8* %buffer, i64 8
  %strncmp.constant = getelementptr inbounds [7 x i8], [7 x i8]* @prefix_value, i64 0, i64 0
  %strncmp.result = call i32 @strncmp(i8* %strncmp.input, i8* %strncmp.constant, i64 3)

  %memcmp.input = getelementptr inbounds i8, i8* %buffer, i64 16
  %memcmp.constant = getelementptr inbounds [5 x i8], [5 x i8]* @binary_value, i64 0, i64 1
  %memcmp.result = call i32 @memcmp(i8* %memcmp.input, i8* %memcmp.constant, i64 3)

  %needle.constant = getelementptr inbounds [8 x i8], [8 x i8]* @needle_value, i64 0, i64 1
  %needle.result = call i8* @strstr(i8* %buffer, i8* %needle.constant)

  switch i32 %selector, label %done [
    i32 287454020, label %case
  ]

case:
  br label %done

done:
  ret i32 0
}
