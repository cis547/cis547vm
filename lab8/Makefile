MAKEFLAGS += --no-builtin-rules

all: submit

# llm/ goes in as one llm.tar.gz because Gradescope uploads each file in a zip separately.
submit:
	chown -R --reference=CMakeLists.txt .
	rm -f /tmp/submission.zip /tmp/llm.tar.gz
	tar -czf /tmp/llm.tar.gz --exclude='.DS_Store' llm
	zip -r /tmp/submission.zip include src test/*.c CMakeLists.txt lab8_writeup.md \
		-x '*.DS_Store' 2> /dev/null
	zip -j /tmp/submission.zip /tmp/llm.tar.gz
	rm -f /tmp/llm.tar.gz
	mv /tmp/submission.zip ./submission.zip
	chown --reference=CMakeLists.txt submission.zip
	echo "submission.zip created successfully."

clean:
	@(cd test; make clean) > /dev/null
	rm -rf build
	rm -f submission.zip