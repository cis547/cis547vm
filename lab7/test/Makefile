TARGETS=simple0 simple1 fuzz0 fuzz1 fuzz2 fuzz3

all: ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load ../build/InstrumentPass.so -Instrument -S $@.ll -o $@.instrumented.ll
	opt -load ../build/CBIInstrumentPass.so -CBIInstrument -S $@.instrumented.ll -o $@.cbi.instrumented.ll
	clang -o $@ -L${PWD}/../build -lruntime $@.cbi.instrumented.ll

clean:
	rm -f *.ll *.cov *.cbi ${TARGETS}
