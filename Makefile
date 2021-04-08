# Makefile for lab 4 about expression trees, March 2021

CC     = gcc
CFLAGS = -std=c99 -pedantic -Wall

prefix: scanner.c recognizeExp.c evalExp.c prefixExp.c mainPref.c
	$(CC) $(CFLAGS) $^ -o $@


infix: scanner.c recognizeExp.c evalExp.c  infixExp.c mainInfix.c
	$(CC) $(CFLAGS) $^ -o $@

part1-test: infix
	cat example_part1_input.txt | ./infix

part1-debug: infix
	cat example_part1_input.txt | valgrind --leak-check=full ./infix


diff: scanner.c recognizeExp.c evalExp.c prefixExp.c infixExp.c diff.c mainDiff.c
	$(CC) $(CFLAGS) $^ -o $@

part2-test: diff
	cat example_part2_input.txt | ./diff

part2-debug: diff
	cat example_part2_input.txt | valgrind --leak-check=full ./diff


clean:
	rm -f pref infix diff
