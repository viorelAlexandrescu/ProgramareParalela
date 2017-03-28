#sectiunea de declarare a 
#variabilelor de mediu shell pentru compilare

#standard C compiler
CC=/usr/bin/gcc
#compiler flag (optimazation level 2)
CFLAGS=-O2

#LIBDIR=/path/to/libs
LIBS=-lgslcblas -lgsl -lm

#INCDIR=/path/to/include/files
####################################################

gplot: gplot.h
	$(CC) -c -o gplot.o gplot.c -I./

bessel: gplot.h gplot.o
	$(CC) -o bessel bessel.c gplot.o $(LIBS)

clean: 
	rm -f gplot.o bessel

#define all actions
all: gplot bessel
