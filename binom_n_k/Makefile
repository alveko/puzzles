CXX	= g++
CF_OPT	= -std=c++11 -c -Wall
CF_REL	= -O3
CF_DBG	= -g -DDEBUG
CF_INC	= -I/usr/local/include -I../..
CFLAGS	= $(CF_OPT) $(CF_REL) $(CF_INC)

#LDFLAGS	= -L/usr/local/lib -lboost_program_options

SRC	= binom_n_k.cc
OBJ	= $(SRC:.cc=.o)
EXE	= binom_n_k

.PHONY: all clean test
all:	$(SRC) $(EXE)
debug:	CF_REL = $(CF_DBG)
debug:	all

$(EXE): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

.cc.o:
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE) *.o
