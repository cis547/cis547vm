TARGETS=simple0 simple1 fuzz0 fuzz1 fuzz2

all: ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load ../build/InstrumentPass.so -Instrument -S $@.ll -o $@.instrumented.ll
	clang -o $@ -L${PWD}/../build -lruntime $@.instrumented.ll

clean:
	rm -f *.ll *.cov ${TARGETS}
