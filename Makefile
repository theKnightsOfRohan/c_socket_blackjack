SIDE ?=
LOG_FILE ?=

all:
	make build run

build: 
	make compile link

unit_test:
	make clean build_test run_test

build_test:
	make compile_test link_test

compile:
	mkdir -p obj/$(SIDE)
	mkdir -p bin/
	mkdir -p log/$(SIDE)
	for file in $(wildcard src/$(SIDE)/*.c); do \
		filename=$$(basename $$file); \
		gcc -Wall -g -c -o obj/$(SIDE)/$(notdir $${filename%.*}.o) $$file; \
	done

	gcc -Wall -g -c -o obj/$(SIDE)/globals.o src/globals.c

compile_test:
	mkdir -p obj/test/$(SIDE)
	mkdir -p bin/test
	mkdir -p log/test/$(SIDE)
	for file in $(wildcard src/$(SIDE)/*.c); do \
		filename=$$(basename $$file); \
		if [ "$$filename" != "main.c" ]; then  \
			gcc -Wall -g -c -o obj/test/$(SIDE)/$(notdir $${filename%.*}.o) $$file; \
		fi \
	done

	for file in $(wildcard test/$(SIDE)/*.c); do \
		filename=$$(basename $$file); \
		gcc -Wall -g -c -o obj/test/$(SIDE)/$(notdir $${filename%.*}.o) $$file; \
	done

	gcc -Wall -g -c -o obj/test/$(SIDE)/globals.o src/globals.c

link:
	gcc -Wall -o bin/$(SIDE) $(wildcard obj/$(SIDE)/*.o)

link_test:
	gcc -Wall -o bin/test/$(SIDE) $(wildcard obj/test/$(SIDE)/*.o)

run:
	./bin/$(SIDE) $(LOG_FILE)

run_test:
	./bin/test/$(SIDE) $(LOG_FILE)

debug:
	make build

	if [ ! -f debug/$(SIDE)/setup.lldb ]; then \
		mkdir -p debug/$(SIDE); \
		touch debug/$(SIDE)/setup.lldb; \
	fi

	lldb ./bin/$(SIDE)

debug_test:
	make build_test

	if [ ! -f debug/test/$(SIDE)/setup.lldb ]; then \
		mkdir -p debug/test/$(SIDE); \
		touch debug/test/$(SIDE)/setup.lldb; \
	fi

	lldb ./bin/test/$(SIDE)


clean:
	rm -rf bin/*
	rm -rf obj/*
	rm -rf log/*
