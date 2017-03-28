#include <mpi.h>

/*
	Initializeaza mediul MPI
	Defineste comunicatorul Standard (MPI COMM WORLD).
	Nici o alta functie MPI nu poate fi apelata inaintea acesteia!
*/
int MPI_Init(int *argc, char ***argv);

/*
	Extrage in size(transmis prin param size) numarul de procese 
	concurente lansate in executie din comunicatorul comm

	@param size comm
 */
int MPI_Comm_size(MPI_Comm comm, int *size);

/*
	Stocheaza in rank rangul procesul care il apeleaza in comunicatorul comm
 */
int MPI_Comm_rank(MPI_Comm comm, int *rank);


/*
	Termina mediul MPI si dezaloca resursele alocate
 */
int MPI_Finalize();

/*
	Distruge mediul MPI asociat comunicatorului comm si intoarce sistemului de operare codul rc
 */
int MPI_Abort(MPI_Comm, int rc);

//MPI_SUCCESS -> int return value of functions -> 0