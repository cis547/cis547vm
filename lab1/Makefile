TARGETS=test1 test2 test3 test4 test5 test6 test7

.PRECIOUS: results/afl_logs/%/out.txt results/ikos_logs/%_int_out.txt results/ikos_logs/%_dbm_out.txt

all: ${TARGETS}

results/afl_logs/%/out.txt: c_programs/%.c
	mkdir -p afl_output/
	AFL_DONT_OPTIMIZE=1 afl-gcc $< $ -o $*
	mkdir -p results/afl_logs/$*
	-timeout 30s afl-fuzz -i afl_input -o afl_output ./$*  > $@
	cp -r afl_output/ $* results/afl_logs/$*/ && rm -rf afl_output/* $*

results/ikos_logs/%_int_out.txt: c_programs/%.c
	mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d interval $< > $@

results/ikos_logs/%_dbm_out.txt: c_programs/%.c
	mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d dbm $< > $@

%: results/afl_logs/%/out.txt results/ikos_logs/%_int_out.txt results/ikos_logs/%_dbm_out.txt
	

submit:	answers.txt results/
	mkdir -p submission
	rm -rf submission/*
	cp -r answers.txt results c_programs submission
	zip -r submission.zip submission/ 2>&1 >/dev/null

clean:
	rm -rf results/*_logs/* ${TARGETS} output.db submission submission.zip afl_output/*
