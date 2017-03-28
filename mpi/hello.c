#include <mpi.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char** argv){
	int my_rank; //rangul procesului curent
	int nproc;//numarul de procese concurente lansate in executie
	int rc;//cod de eroare pentru functii MPI

	// 2.5 * sin(3.14*(double)rank))/((double)nproc)

	double value;
	MPI_Comm my_comm = MPI_COMM_WORLD;

	//init MPI environment
	rc = MPI_Init(&argc, &argv);

	if(rc != MPI_SUCCSESS){
		printf("Failed to init MPI\n");
		MPI_Abort(my_comm, rc);
	}


	MPI_Comm_size(my_comm, &nproc);
	MPI_Comm_rank(my_comm, &my_rank);

	val = 2.5*sin(3.14*(double)my_rank)/((double)nproc);
	printf("Procesul %d: val = %5lf",my_rank, value);

	MPI_Finalize();

	return 0;
}