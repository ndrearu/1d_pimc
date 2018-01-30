CC=g++
CFLAGS=-O3 -Wno-unknown-pragmas -Wextra -pedantic -std=c++11 -fopenmp

SRC  = src/1d_pimc.cc
EXE  = bin/1d_pimc
LIBS = src/iostruct.cc\
			 src/pathclass.cc\
			 src/pimcclass.cc\
			 src/potentials.cc

1d_pimc: $(SRC) $(LIBS)
	$(CC) $(CFLAGS) -o $(EXE) $(SRC) $(LIBS)
