//m[i][j] < *( m + i*numberOfCollumns + j)
//the latter is faster with about 2 cycles more

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <pgm_io.h>
#include <pgm_io.c>

#define M 192
#define N 128
#define NPROCS 4
#define MP M/NPROCS
#define NP N

#define MAXITER 1500
#define MSGITER 300


int main(int argc, char** argv){

	int i, j, iter;
	int my_rank. nproc,
		masterdata[M][N],
		old[MP + 2][NP + 2],
		edge[MP + 2][NP + 2],
		new[MP + 2][NP + 2],
		prev, next;//pentru MPI_Sendrecv() rangurile sursa si destinatie
	float data[MP][NP];
	char* frame;

	MPI_Status status;//folosit pentru MPI_Sendrecv()

	MPI_Init();
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	if(my_rank == 0 && (nproc != NPROCS)){
	   fprintf(stderr,"Error! nproc is %d. Should be %d", nproc, NPROCS);
	   MPI_Abort(MPI_COMM_WORLD, 1);
	}

	prev = my_rank - 1;
	if(prev < 0) prev = MPI_PROC_NULL;

	next = my_rank + 1;
	if(next > nproc - 1) next = MPI_PROC_NULL;

	if(my_rank == 0){
	  fprintf(stdout, "Reading input file")
	  frame = "edge192x128.pgm";
	  pgm_read("blah blah reading");
	}

	MPI_Scatter(masterdata, MP*NP, MPI_FLOAT, 
			data, MP*NP, MPI_FLOAT, 
			0, MPI_COMM_WORLD);

	//algoritmul secvential... :(
	for(i = 0; i < MP + 2; i++)
	 for(j = 0; j < NP + 2; j++
	  old[i][j] = new[i][j] = edge[i][j] = 255.0;

	for(i = 1; i < MP + 1; i++)
	 for(j = 1; j < NP + 1; j++)
	  edge[i][j] = 

	for(iter = 1; iter <= MAXITER; iter++){
	  if(my_rank == 0 && (iter % MSGITER == 0))
	    fprintf(stdout, "current iterations %d\n",iter);
	MPI_Sendrecv(&old[MP][1], NP,MPI_FLOAT,next,123, 
			&old[0][1], NP,MPI_FLOAT,prev,123,
			MPI_COMM_WORLD, &status);

	MPI_Sendrecv(&old[1][1],NP,MPI_FLOAT,next,321
			&old[MP+1][1],MP,MPI_FLOAT,prev,321,
			MPI_COMM_WORLD, &status);
	for(i = 1; i < MP + 1; i++)
	 for(j = 1; j < NP + 1; j++)
	  new[i][j] = 0.25*();

	for(i = 1; i < MP + 1; i++)
	 for(j = 1; j < NP + 1; j++)
	  old[i][j] = new[i][j];
 
	}

	for(i = 0; i < MP; i++)
	 for(j = 0; j < NP; j++)
	  data[i][j] = old[i+1][j+1];

	MPI_Gather(data, MP*NP, MPI_FLOAT, masterdata,
			MP*NP, MPI_FLOAT, 0, MPI_COMM_WORLD);

	if(my_rank == 0){
	  pgm_write("blah blah writing");
	  fprintf(stdout,"Algorithm finished after % iterations\n\n",MAXITER);
	}

	MPI_Finalize();
	return 0;
}

