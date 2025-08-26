; ModuleID = 'test4.c'
source_filename = "test4.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %call = call i32 @getchar()
  store i32 %call, i32* %a, align 4
  %0 = load i32, i32* %a, align 4
  %cmp = icmp eq i32 %0, 1
  br i1 %cmp, label %if.then, label %if.end12

if.then:                                          ; preds = %entry
  %call1 = call i32 @getchar()
  store i32 %call1, i32* %a, align 4
  %1 = load i32, i32* %a, align 4
  %cmp2 = icmp eq i32 %1, 3
  br i1 %cmp2, label %if.then3, label %if.end11

if.then3:                                         ; preds = %if.then
  %call4 = call i32 @getchar()
  store i32 %call4, i32* %a, align 4
  %2 = load i32, i32* %a, align 4
  %cmp5 = icmp eq i32 %2, 5
  br i1 %cmp5, label %if.then6, label %if.end10

if.then6:                                         ; preds = %if.then3
  %call7 = call i32 @getchar()
  store i32 %call7, i32* %a, align 4
  %3 = load i32, i32* %a, align 4
  %cmp8 = icmp eq i32 %3, 7
  br i1 %cmp8, label %if.then9, label %if.end

if.then9:                                         ; preds = %if.then6
  %4 = load i32, i32* %a, align 4
  %div = sdiv i32 1, %4
  store i32 %div, i32* %a, align 4
  br label %if.end

if.end:                                           ; preds = %if.then9, %if.then6
  br label %if.end10

if.end10:                                         ; preds = %if.end, %if.then3
  br label %if.end11

if.end11:                                         ; preds = %if.end10, %if.then
  br label %if.end12

if.end12:                                         ; preds = %if.end11, %entry
  ret i32 0
}

declare i32 @getchar() #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+outline-atomics,+v8a" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+outline-atomics,+v8a" }

!llvm.module.flags = !{!0, !1, !2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"branch-target-enforcement", i32 0}
!2 = !{i32 1, !"sign-return-address", i32 0}
!3 = !{i32 1, !"sign-return-address-all", i32 0}
!4 = !{i32 1, !"sign-return-address-with-bkey", i32 0}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 1}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"Ubuntu clang version 14.0.6"}
