SIDE ?=

all:
	make build run

build:
	make compile link

compile:
	mkdir -p obj/$(SIDE)/
	mkdir -p bin/
	mkdir -p log/
	for file in $(wildcard src/$(SIDE)/*.c); do \
		filename=$$(basename $$file); \
		gcc -Wall -g -c -o obj/$(SIDE)/$(notdir $${filename%.*}.o) $$file; \
	done

	gcc -Wall -g -c -o obj/$(SIDE)/globals.o src/globals.c

link:
	gcc -Wall -o bin/$(SIDE) $(wildcard obj/$(SIDE)/*.o)
	chmod +x bin/$(SIDE)

run:
	./bin/$(SIDE) $(SIDE)

debug:
	make build

	if [ ! -f src/$(SIDE)/.debug/setup.lldb ]; then \
		touch src/$(SIDE)/.debug/setup.lldb; \
	fi

	lldb ./bin/$(SIDE) -s src/$(SIDE)/.debug/setup.lldb

clean:
	rm -rf bin/*
	rm -rf obj/*
	rm -rf log/*
