#include "gol.h"
#include <pgm_io.h>
#include <pgm_io.c>

#define MASTER 0

int main(int argc, char** argv){

	int source, dest = 0, my rank, nproc, tag = 1,
		NX, NY, i, j,
		dims[NDIM] = (int*)calloc(NDIM*sizeof(int)),
		GSIZE = XSIZE * YSIZE,
		reorder = FALSE, periods[NDIM],
		deltaX, deltaY, BSize,
		coords[NDIM] = (int*)calloc(NDIM*sizeof(int));
	float *garray, *barray;


	// init periods with TRUE value
	for(int i = 0; i < NDIM; i++) *(periods + i) = TRUE;

	MPI_Comm comm = MPI_COMM_WORLD, cartcomm;
	MPI_Status status;

	MPI_Datatype block_t;
	char frame[64] = (char*)calloc(64*sizeof(char));

	//Initializare MPI
	MPI_Init();
	MPI_Comm_size(comm, &nproc);
	MPI_Comm_rank(comm, &my_rank);

	//Distributia optima a proceselor de topologie virtuala 2D
	MPI_Dims_create(nproc, NDIM, dims);
	MPI_Cart_create(comm, NDIM, dims, periods, reorder, &cartcomm);

	NX = dims[0];
	NY = dims[1];

	BSize = GSIZE / NX / NY;

	if(my_rank == MASTER){
		garray = (float*) malloc(GSIZE*sizeof(float));

	for(i = 0; i < GSIZE; i++)
		*(garray + i) = 0.75 * MAXGREY;
	}

	harray = (float*) malloc(BSize*sizeof(float));
	for(i = 0; i < BSize; i++)
		*(harray + i) = 0.72 * MAXGREY;
	
}

