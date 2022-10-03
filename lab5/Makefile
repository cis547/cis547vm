MAKEFLAGS += --no-builtin-rules

PY_SRC:=$(shell find . -name '*.py') requirements.txt
C_SRC=$(shell find src -name '*.cpp') $(shell find . -name '*.h') $(shell find ./lib -name '*.c')
SRC=${PY_SRC} ${C_SRC} Makefile CMakeLists.txt

all: install

build: ${C_SRC}
	@echo "Building Instrumentation..."
	@(mkdir -p ./build; cd ./build; cmake .. && make)

install: build ${PY_SRC}
	@echo "Installing CBI..."
	@python3 -m pip install --upgrade --editable . 1> /dev/null 2>&1
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
	rm -rf ./build

clean-package:
	@echo "Cleaning cbi..."
	@python3 -m pip uninstall cbi 2> /dev/null
	@rm -f /usr/local/bin/cbi
	@rm -rf */__pycache__ *.egg-info

clean-test:
	@echo "Cleaning up test..."
	@(cd test; make clean)

clean: clean-package clean-instrumentation
	@echo "Removing submission.zip"
	rm -f ./submission.zip
