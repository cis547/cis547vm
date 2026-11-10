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

LLM_RELATIONS:=def use next taint sanitizer div

%-llm:
	@for relation in $(LLM_RELATIONS); do \
		test -f ../llm/facts/$*/$$relation.facts || \
			{ echo "../llm/facts/$*/$$relation.facts not found; run llm/llm_extract.py first" >&2; exit 1; }; \
	done
	@mkdir -p $*/llm-output
	souffle -F ../llm/facts/$* -D $*/llm-output ../src/llm_analysis.dl

clean:
	rm -rf *.ll ${TARGETS}
