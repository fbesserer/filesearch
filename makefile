CC=gcc
CFLAGS=-Wall
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS)) #für alle SRCS substituiere %.c mit %.o

TEST=tests
TESTBINDIR=$(TEST)/bin
TESTS=$(wildcard $(TEST)/*.c)
TESTBINS=$(patsubst $(TEST)/%.c, $(TEST)/bin/%, $(TESTS))

BINDIR=bin
BIN=$(BINDIR)/main

all: $(BIN)

release: CFLAGS=-Wall -O2 #-O2 enables Compiler optimization (should only be enabled after testing)
release: clean
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

hashtable: $(SRC)/hashtable.c $(SRC)/hashtable.h
	$(CC) $(CFLAGS) $^ -o $@

$(TESTBINDIR): 
	mkdir $@

$(TESTBINDIR)/%: $(TEST)/%.c $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJ)/hashtable.o -o $@ -lcriterion

test: $(TESTBINDIR) $(TESTBINS)
	for test in $(TESTBINS); do ./$$test; done 

clean:
	$(RM) $(BINDIR)/* $(OBJ)/*
cleantests:
	$(RM) $(TESTBINDIR)/*