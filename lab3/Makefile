MAKEFLAGS += --no-builtin-rules

SRC:=$(shell find ./src -type f -name "*.cpp")

.PHONY: all build check-dictionary check-dictionary-application check-cmplog check-power-schedule clean submit validate-json-seeds

all: submit

build:
	cmake -S . -B build
	cmake --build build

check-dictionary: build
	@python3 -B tools/dictionary_tools.py check \
		--plugin build/FuzzingAnalysis.so \
		--fixture test/dictionary_public.ll

check-dictionary-application: build
	cmake --build build --target dictionary_application_check
	./build/dictionary_application_check

check-cmplog: build
	@python3 -B tools/cmplog_tools.py check \
		--plugin build/FuzzingAnalysis.so \
		--runtime build/libruntime.so \
		--fixture test/cmplog_public.ll

check-power-schedule: build
	cmake --build build --target power_schedule_check
	./build/power_schedule_check

validate-json-seeds:
	@python3 -B tools/json_seed_tools.py validate json_seeds

submit: validate-json-seeds ${SRC} clean
	chown -R --reference=CMakeLists.txt .
	zip -r /tmp/submission.zip * 2> /dev/null
	mv /tmp/submission.zip ./submission.zip
	chown --reference=CMakeLists.txt submission.zip
	echo "submission.zip created successfully."

clean:
	rm -rf build tools/__pycache__
	(cd test; make clean) > /dev/null
	rm -f submission.zip
