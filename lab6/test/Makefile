.PRECIOUS: %.ll

TARGETS=simple0.out simple1.out branch0.out loop0.out branch1.out branch2.out loop1.out

all: ${TARGETS}

%.ll: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@ $<

%.out: %.ll
	../build/constraint $< > $@ 2> $*.err

clean:
	rm -f *.ll *.out *.err ${TARGETS}
