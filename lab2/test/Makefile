TARGETS=simple0 simple1 simple2 simple3 simple4 simple5 simple6 simple7 simple8 simple9

all: ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load ../build/InstrumentPass.so -Instrument -S $@.ll -o $@.instrumented.ll
	clang -o $@ -L${PWD}/../build -lruntime $@.instrumented.ll

clean:
	rm -f *.ll *.cov ${TARGETS}
