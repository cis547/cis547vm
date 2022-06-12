TARGETS:=$(shell find . -type f -name "*.c" -exec basename -s .c -a {} \;)

all: ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load ../build/InstrumentPass.so -Instrument -S $@.ll -o $@.instrumented.ll
	opt -load ../build/CBIInstrumentPass.so -CBIInstrument -S $@.instrumented.ll -o $@.cbi.instrumented.ll
	clang -o $@ -L${PWD}/../build -lruntime -lm $@.cbi.instrumented.ll

fuzz-%: %
	@./test.sh $< 10s

clean:
	rm -rf *.ll *.cov *.jsonl *.json core.* fuzz_output_* ${TARGETS}
