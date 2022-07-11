.PRECIOUS: %.ll
MAKEFLAGS += --no-builtin-rules

SRC:=$(wildcard *.c)
TARGETS:=$(patsubst %.c, %, $(SRC))

all: ${TARGETS}
	@head -n100 */output/alarm.csv

%.ll: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@ $<

%: %.ll
	@mkdir -p $@/input $@/output
	../build/constraint $< $@/input 2> $@/err > $@/out
	souffle -F $@/input -D $@/output ../src/analysis.dl

clean:
	rm -rf *.ll ${TARGETS}
