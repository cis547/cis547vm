MAKEFLAGS += --no-builtin-rules
CFLAGS = ""

TARGETS=test1 test2 test3 test4

all: $(TARGETS)

%.ll: %.c
	clang $(CFLAGS) -emit-llvm -S -fno-discard-value-names -c $<

%: %.ll
	diff -y --suppress-common-lines --report-identical-files --ignore-all-space $@.ll ../ir_programs/$@.ll
	clang -o $@ $@.ll

clean:
	rm -f *.ll $(TARGETS)

