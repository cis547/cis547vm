TARGETS=test6

.PRECIOUS: results/afl_logs/%/out.txt results/ikos_logs/%_int_out.txt results/ikos_logs/%_dbm_out.txt

all: ${TARGETS}

results/afl_logs/%/out.txt: c_programs/%.c
	mkdir -p results/afl_logs/$*
	AFL_DONT_OPTIMIZE=1 afl-gcc $< $ -o results/afl_logs/$*/$*
	-timeout 30s afl-fuzz -i afl_input -o results/afl_logs/$*/out_dir results/afl_logs/$*/$* > $@

results/ikos_logs/%_int_out.txt: c_programs/%.c
	mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d interval $< > $@

results/ikos_logs/%_dbm_out.txt: c_programs/%.c
	mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d dbm $< > $@

%: results/afl_logs/%/out.txt results/ikos_logs/%_int_out.txt results/ikos_logs/%_dbm_out.txt
	

submit:	f1_scores.txt compare.txt results/
	mkdir -p submission
	rm -rf submission/*
	cp -r f1_scores.txt results c_programs submission
	zip -r submission.zip submission/ 2>&1 >/dev/null

clean:
	rm -rf results/*_logs/* ${TARGETS} output.db submission submission.zip afl_output/*
