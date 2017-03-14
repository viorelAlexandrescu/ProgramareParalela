#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*
	toate procesele vor calcula 0.32*sin(3*PI/(double)rank)
	procesul 0 (master): va crea un array de nproc-1 double
*/

#define MASTER	0 //rangul procesului master
//#define SQR(x) (x*x)
//#define MAX(x,y) ((x >=y)?x:y))

int main(int argc, char **argv){

	int my_rank,	// rangul procesului curent
		nproc,	// nr de procese lansate in executie
		tag=100,// eticheta mesajelor de comunicare punct-la-punct
		src,	// rangul proceselor sursa ale mesajelor
		iterator;
	MPI_Status status;//starea proceselor de comunicare

	MPI_Comm comm_world = MPI_COMM_WORLD;

	double result,*data;

	//initializare mediului paralele de executie
	MPI_Init(&argc, &argv);

	MPI_Comm_size(comm_world, &nproc);
	MPI_Comm_rank(comm_world, &my_rank);

	printf("Procesul %d a pornit!\n",my_rank);
	if(my_rank != MASTER){
	result = 0.32*exp((-3*M_PI)/((double)my_rank));
	 MPI_Send(&result, 1, MPI_DOUBLE, MASTER, tag, comm_world);
	}
	if(my_rank == MASTER){
	 data = (double*)malloc((nproc-1)*sizeof(double));
	 for(src = 1; src < nproc; src ++)
	  MPI_Recv(data + src -1, 1, MPI_DOUBLE, src, tag, comm_world, &status);

	printf("\n Procesul %d:\n", MASTER);
	for(iterator =  0; iterator < nproc-1; iterator++)
	 printf("Valoarea %d: %.8lf\n",iterator+1,*(data+iterator));
	// later add conditions to testing if there is not enough memory
	// exit with MPI_Abort();
	free(data);
	}

 MPI_Finalize();
 return 0;
}
