SIDE ?=

all:
	make build run

build:
	make compile link

compile:
	mkdir -p obj/$(SIDE)/
	mkdir -p bin/
	for file in $(wildcard src/$(SIDE)/*.c); do \
		filename=$$(basename $$file); \
		gcc -Wall -g -c -o obj/$(SIDE)/$(notdir $${filename%.*}.o) $$file; \
	done

link:
	gcc -Wall -o bin/$(SIDE) $(wildcard obj/$(SIDE)/*.o)
	chmod +x bin/$(SIDE)

run:
	./bin/$(SIDE)

debug:
	lldb ./bin/$(SIDE)

clean:
	rm -rf bin/*
	rm -rf obj/*
