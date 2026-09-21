TARGETS := $(basename $(notdir $(wildcard *.c)))
GENERATED_LL := $(TARGETS:%=%.ll) $(TARGETS:%=%.instrumented.ll)

.PHONY: all clean $(TARGETS)

all: $(TARGETS:%=%.out)

# Keep the original student-facing target names while using .out to distinguish
# generated executables from their C sources.
$(TARGETS): %: %.out

%.out: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $*.ll $< -g
	opt -load-pass-plugin=../build/FuzzingAnalysis.so -passes="FuzzingAnalysis" -S $*.ll -o $*.instrumented.ll
	clang -o $@ -L$(dir $(lastword $(MAKEFILE_LIST)))/../build -Wl,-rpath,../build -lruntime -lm $*.instrumented.ll
	mv fuzzing_dict fuzzing_dict_$*

json_parser_benchmark.out: \
	vendor/json-parser/json.c \
	vendor/json-parser/json.h

fuzz-json_parser_benchmark: json_parser_benchmark.out
	@./test.sh json_parser_benchmark 30s benchmark_input/json

fuzz-%: %.out
	@./test.sh $* 10s

clean:
	rm -rf $(GENERATED_LL) *.cov ${TARGETS} core.* fuzz_output* output_*.txt *.out fuzzing_dict*
