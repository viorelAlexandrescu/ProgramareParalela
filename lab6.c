#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define MASTER 0
#define ITER 100
#define TRIES 6000

#define SQR(x) (x*x)

double hit(int tries);

int main(int argc, char **argv){
	int nproc,
		my_rank,
		rc, //return code pentru functii MPI <- test
		i;

	double local_pi,// valoarea locala pe procesul curent
			aver_pi,// valoare pi medie dupa un nr de incercari
			sum_pi, // suma valorilor locale
			pi; 	// valoarea medie la sfarsit

	MPI_Status status;

	MPI_Init(&argc,args);
	fprintf("stdout","Procesul %i a pornit\n",my_rank);
	
	aver_pi = 0;
	srandom(my_rank);
	/* Nr Itr | Nr incercari | Val pi */
	if(my_rank == MASTER)
		printf("Nr iter\tNr Incercari\tVal Pi\n")
	for(i = 1; i <= ITER; i++){
		local_pi = hit(TRIES);
		rc = MPI_Reduce(&local_pi, &sum_pi, 1, 
				MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
		fprintf("stdout","Procesul %d: eroare in operatia MPI_Reduce, cod %d\n",my_rank,rc);
		if(my_rank == MASTER){
			pi = sum_pi / nproc;
			aver_pi = (aver_pi * i + pi) / (i + 1);
			printf("%d\t %10.8lf \n", TRIES*i, aver_pi);
		}
	}
	if(my_rank == MASTER)
		printf("Valoarea pi aproximatativa: %10.8lf\n\n",4.0*atan(1));

	MPI_Finalize();

	return 0;
}

double hit(int tries){
	double pi, x_local, y_local, r;
	int n, n_in = 0;

	//shiffting is better than multiplication or division
    const unsinged int rmconst = 2 << (sizeof(int)*8) - 1;

    for(n = 1; n <= tries; n++){
    	r = (double) (random / rmconst);
        x_local = 2.*r - 1;

        r = (double) (random / rmconst);
        y_local = 2.*r - 1;

        if( (SQR(x_local) + SQR(y_local)) <= 1){
        	n_in++;
        }
    }

    pi = ((double) n_in) / ((double ) tries)*4.0;

    return pi;
}

