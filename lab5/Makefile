MAKEFLAGS += --no-builtin-rules

SRC:=$(shell find . -name '*.py') requirements.txt Makefile CMakeLists.txt $(shell find . -name '*.cpp') $(shell find . -name '*.c') $(shell find . -name '*.h')

all: install

build: ${SRC}
	@echo "Building Instrumentation..."
	@(mkdir -p ./build; cd ./build; cmake .. && make)
	@echo "Building CBI..."
	@python3 -m pip install --upgrade --editable . 1> /dev/null 2>&1

install: build
	@echo "CBI installed."

submit: clean
	@rm -f submission.zip
	@echo "Creating submission..."
	@chown -R --reference=Makefile .
	zip -r /tmp/submission.zip cbi/* setup.py requirements.txt Makefile CMakeLists.txt include/* lib/* src/* 2> /dev/null
	@mv /tmp/submission.zip submission.zip
	@chown -R --reference=Makefile submission.zip
	@echo "submission zip created."

clean-instrumentation:
	@echo "Cleaning instrumentation..."
	@mkdir -p ./build
	rm -rf ./build/*

clean-package:
	@echo "Cleaning cbi..."
	@python3 -m pip uninstall cbi 2> /dev/null
	@rm -f /usr/local/bin/cbi
	@rm -rf */__pycache__ *.egg-info

clean-test:
	@echo "Cleaning up test..."
	@(cd test; make clean)

clean: clean-package clean-test clean-instrumentation
	@echo "Removing submission.zip"
	rm -f ./submission.zip