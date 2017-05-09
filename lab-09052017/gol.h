#ifdef _GOL_H_
#define _GOL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

//focalizarea periodicitatii topologiei virtuale

#define TRUE 1
#define FALSE 0
#define NDIM 2		//dimensiunea topologie virtuale
#define ITER 25		//nr de iteratii avansare algoritm
#define XSIZE 128	//XSIZE * YSIZE pixeli in imaginea pgm
#define YSIZE 128
#define MAXGREY 100	//

typedef struct _LTYPE_{
	int x, y;
}LType;

#endif

