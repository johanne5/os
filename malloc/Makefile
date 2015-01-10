ifdef NOWARNINGS
WARNINGS = -w
else
WARNINGS = -Wall -Wno-deprecated-declarations
endif

ifdef NODEBUG
DEBUG = 
else
DEBUG = -ggdb
endif

#variables
CC = gcc
CFLAGS = $(DEBUG) $(WARNINGS)

#files
DEPS = malloc.h
TEST_SOURCE_FILES = test.c malloc.c
TEST_OBJECT_FILES = $(TEST_SOURCE_FILES:.c=.o)

all: test

test: $(TEST_OBJECT_FILES)
	$(CC) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) $<

clear:
	rm -rf *.o test
