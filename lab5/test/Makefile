TARGETS:=$(shell find . -type f -name "*.c" -exec basename -s .c -a {} \;)

all: build-fuzzer ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load-pass-plugin=../build/InstrumentPass.so -passes="Instrument" -S $@.ll -o $@.instrumented.ll
	opt -load-pass-plugin=../build/CBIInstrumentPass.so -passes="CBIInstrument" -S $@.instrumented.ll -o $@.cbi.instrumented.ll
	clang -o $@ -L${PWD}/../build -lruntime -lm $@.cbi.instrumented.ll

fuzz-%: %
	@./test.sh $< 10s

clean: clean-fuzzer
	rm -rf *.ll *.cov *.jsonl *.json core.* fuzz_output_* ${TARGETS}

.PHONY: build-fuzzer
build-fuzzer:
	@echo "Building Fuzzer..."
	@(cd ../fuzzer; mkdir -p build; cd build; cmake ..; make)

.PHONY: clean-fuzzer
clean-fuzzer:
	@echo "Cleaning up fuzzer build directory..."
	@rm -rf ../fuzzer/build
