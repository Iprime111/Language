.PHONY: compile all

all: compile

FILENAME = while_test.phc

compile:
	@@../build/bin/frontend -s ${FILENAME} -n ../build/Names.tmp -t ../build/Tree.tmp -d ../build/dump.dot
	@@../build/bin/backend -t ../build/Tree.tmp -n ../build/Names.tmp -S ../build/listing.txt > ../build/code.asm
	@@nasm -felf64 ../build/code.asm -o ../build/code.o
	@@ld ../build/code.o -o ../build/code
