MAKEFLAGS += --no-builtin-rules

SRC:=$(shell find . -name '*.py') requirements.txt Makefile

all: install build-fuzzer

build: clean-package build-fuzzer ${SRC} pip-check
	@echo "Building Delta-Debugger..."
	@python3 -m pip install --use-pep517 --upgrade --editable .

pip-check:
	@if ! command -v pip &> /dev/null; then \
		echo "pip not found, installing..."; \
		apt-get update && apt-get install -y python3-pip; \
	fi

install: build
	@echo "Delta-Debugger installed."

build-fuzzer:
	@echo "Building Fuzzer and InstrumentPass..."
	@(cd fuzzer; mkdir -p build; cd build; cmake ..; make)

submit:
	@rm -f submission.zip
	@echo "Creating submission..."
	@chown -R --reference=Makefile .
	zip -r /tmp/submission.zip ${SRC} 2> /dev/null
	@mv /tmp/submission.zip submission.zip
	@chown -R --reference=Makefile submission.zip
	@echo "submission zip created."

clean-package:
	@echo "Cleaning delta-debugger..."
	    @python3 -m pip uninstall --yes delta-debugger 2> /dev/null || true	@rm -f /usr/local/bin/delta-debugger
	@rm -rf */__pycache__ *.egg-info

clean-test:
	@echo "Cleaning up test..."
	@(cd test; make clean)

clean-fuzzer:
	@echo "Cleaning up fuzzer build directory..."
	@rm -rf fuzzer/build

clean: clean-package clean-test clean-fuzzer
	@echo "Removing submission.zip"
	rm -f ./submission.zip