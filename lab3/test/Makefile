TARGETS:=$(shell find . -type f -name "*.c" -exec basename -s .c -a {} \;)

all: $(TARGETS:%=%.out)

%.out: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $*.ll $< -g
	opt -load-pass-plugin=../build/FuzzingAnalysis.so -passes="FuzzingAnalysis" -S $*.ll -o $*.instrumented.ll
	clang -o $@ -L$(dir $(lastword $(MAKEFILE_LIST)))/../build -Wl,-rpath,../build -lruntime -lm $*.instrumented.ll
	mv fuzzing_dict fuzzing_dict_$*

fuzz-%: %.out
	@./test.sh $* 10s

clean:
	rm -rf *.ll *.cov ${TARGETS} core.* fuzz_output* output_*.txt *.out fuzzing_dict*
