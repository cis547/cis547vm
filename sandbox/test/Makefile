TARGETS=simple0

all: ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $<
	opt -load ../build/AnalyticsPass.so -Analytics -S $@.ll -disable-output

clean:
	rm -f *.ll
