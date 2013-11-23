CC=g++
CFLAGS=-std=c++11 -c -Wall -g
LDFLAGS=

SRC=snake.cc
OBJ=$(SRC:.cc=.o)
EXE=snake

.PHONY: all clean test

all: $(SRC) $(EXE)

$(EXE): $(OBJ) 
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE) $(OBJ) *.o

test: clean all
	@for tc in `find tcs -type f | sort`; do \
	    echo "\n***** ./$(EXE) < $$tc"; \
	    ./$(EXE) < $$tc; \
	done
