TARGETS=test0 test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11

.PRECIOUS: results/afl_logs/%/out.txt results/ikos_logs/%_int_out.txt results/ikos_logs/%_dbm_out.txt results/gemini_logs/%_overapprox_out.txt results/gemini_logs/%_underapprox_out.txt

all: ${TARGETS}

results/afl_logs/%/out.txt: c_programs/%.c
	@mkdir -p afl_output/
	AFL_DONT_OPTIMIZE=1 AFL_USE_UBSAN=1 afl-clang-fast -fsanitize=integer-divide-by-zero $< $ -o $* 2> /dev/null
	@mkdir -p results/afl_logs/$*
	-AFL_NO_UI=1 timeout 30s afl-fuzz -i afl_input -o afl_output -m none ./$*  > $@ 2> /dev/null
	@cp -r afl_output/ $* results/afl_logs/$*/ && rm -rf afl_output/* $*

results/ikos_logs/%_int_out.txt: c_programs/%.c
	@mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d interval $< > $@ 2> /dev/null

results/ikos_logs/%_dbm_out.txt: c_programs/%.c
	@mkdir -p results/ikos_logs
	ikos --opt none -a dbz -d dbm $< > $@ 2> /dev/null

results/gemini_logs/%_overapprox_out.txt: c_programs/%.c
	@mkdir -p results/gemini_logs
	# Sandboxing to avoid information leaking (Gemini will cheat off other tools)
	$(eval TMPDIR := $(shell mktemp -d))
	@cp $< $(TMPDIR)/$(notdir $<)
	cd $(TMPDIR) && gemini -p "analyze @$(notdir $<) and report any divide-by-zero errors (YES or NO). Prefer false positives over false negatives." > $(CURDIR)/$@

results/gemini_logs/%_underapprox_out.txt: c_programs/%.c
	@mkdir -p results/gemini_logs
	# Sandboxing to avoid information leaking (Gemini will cheat off other tools)
	$(eval TMPDIR := $(shell mktemp -d))
	cp $< $(TMPDIR)/$(notdir $<)
	cd $(TMPDIR) && gemini -p "analyze @$(notdir $<) and report any divide-by-zero errors (YES or NO). Prefer false negatives over false positives." > $(CURDIR)/$@


%: results/afl_logs/%/out.txt results/ikos_logs/%_int_out.txt results/ikos_logs/%_dbm_out.txt results/gemini_logs/%_overapprox_out.txt results/gemini_logs/%_underapprox_out.txt
	@echo "Making " $<

submit:	answers.txt results/
	mkdir -p submission
	rm -rf submission/*
	cp -r answers.txt results c_programs submission
	@chown -R --reference=Makefile submission
	zip -r submission.zip submission/ 2>&1 >/dev/null
	@chown -R --reference=Makefile submission.zip

clean:
	rm -rf results ${TARGETS} output.db submission submission.zip afl_output

