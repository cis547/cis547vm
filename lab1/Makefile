TARGETS=test0 test1 test2 test3 test4 test5 test6 test7 test8 test9

.PRECIOUS: results/afl_logs/%/out.txt results/ikos_logs/%_int_out.txt results/ikos_logs/%_dbm_out.txt

all: ${TARGETS}

results/afl_logs/%/out.txt: c_programs/%.c
	@mkdir -p afl_output/
	AFL_DONT_OPTIMIZE=1 afl-gcc $< $ -o $* 2> /dev/null
	@mkdir -p results/afl_logs/$*
	-timeout 30s afl-fuzz -i afl_input -o afl_output -m none ./$*  > $@ 2> /dev/null
	@cp -r afl_output/ $* results/afl_logs/$*/ && rm -rf afl_output/* $*

results/ikos_logs/%_int_out.txt: c_programs/%.c
	@mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d interval $< > $@ 2> /dev/null

results/ikos_logs/%_dbm_out.txt: c_programs/%.c
	@mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d dbm $< > $@ 2> /dev/null

%: results/afl_logs/%/out.txt results/ikos_logs/%_int_out.txt results/ikos_logs/%_dbm_out.txt
	

submit:	answers.txt results/
	mkdir -p submission
	rm -rf submission/*
	cp -r answers.txt results c_programs submission
	@chown -R --reference=Makefile submission
	zip -r submission.zip submission/ 2>&1 >/dev/null
	@chown -R --reference=Makefile submission.zip

clean:
	rm -rf results ${TARGETS} output.db submission submission.zip afl_output
