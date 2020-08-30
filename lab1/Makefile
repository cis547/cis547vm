TARGETS=test1 test2 test3 test4 fuzz_test1 fuzz_test2 sv-adapt

.PRECIOUS: results/afl_logs/%/afl.out results/ikos_logs/%.ikos.int.out results/ikos_logs/%.ikos.dbm.out

all: ${TARGETS}

results/afl_logs/%/afl.out: c_programs/%.c
	AFL_DONT_OPTIMIZE=1 afl-gcc $< $ -o $*
	mkdir -p results/afl_logs/$*
	-timeout 30s afl-fuzz -i afl_input -o afl_output ./$*  > $@
	cp -r afl_output/ $* results/afl_logs/$*/ && rm -rf afl_output/* $*

results/ikos_logs/%.ikos.int.out: c_programs/%.c
	mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d interval $< > $@

results/ikos_logs/%.ikos.dbm.out: c_programs/%.c
	mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d dbm $< > $@

%: results/afl_logs/%/afl.out results/ikos_logs/%.ikos.int.out results/ikos_logs/%.ikos.dbm.out
	

submit:	f1_scores.txt compare.txt results/
	mkdir -p submission
	rm -rf submission/*
	cp -r f1_scores.txt results c_programs submission
	zip -r submission.zip submission/ 2>&1 >/dev/null

clean:
	rm -rf results/*_logs/* ${TARGETS} output.db submission submission.zip afl_output/*
