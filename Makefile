# Makefile

CC=gcc

EXCLUDE_FILES = test.c

FILES = $(shell find *.c -not -name ${EXCLUDE_FILES})

FLAGS= $(FILES) -o lang -O2 -std=c99 -Iinclude

TEST_FILE = test/test2.lang

TEST_OUT_FILE = test_out.s

ASM_DIR = asm

all: build run run_test

build:
	$(CC) $(FLAGS)

asm_out:
	gcc -S -m32 -O0 -fno-asynchronous-unwind-tables test.c

run:
	./lang $(TEST_FILE) $(ASM_DIR)/$(TEST_OUT_FILE)

run_test:
	cd $(ASM_DIR) && make