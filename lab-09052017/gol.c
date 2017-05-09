#include "gol.h"
#include <pgm_io.h>
#include <pgm_io.c>

#define MASTER 0

/*
	Proiect - Implementare paralela (MPI) a "Jocului vietii"
-----------------------------------------------------------
Obiective:
- utilizarea unui model de programare de tip master-slave
- efectuarea unei descompuneri de domeniu si schimburi de date in regiuni "halo"

Etape
------

1) Intr-o prima etapa veti atinge urmatoarele obiective:
	* Crearea unui model master-worker â€“ procesul master scrie fisierul de date final (o imagine in format pgm).
	* Partitionarea unei matrici intre procese.
	* Generarea unei topologii carteziene virtuale.
	* Fiecare proces va colora sectiunea de date locala in alb sau negru, in functie de pozitia sa in structura topologica.
	* Transmiterea de date catre procesul master, care va scrie fisierul pgm final.
Rezultul trebuie sa fie o imagine de tip tabla de sah.

Pseudo-cod:
	
	  Initializare MPI
	  _
	 | determina numarul de procese lansate in executie
Pasul 1	-| determina nx = numar de procese pe directia x in topologia virtuala
	 |           ny = numar de procese pe directia y in topologia virtuala
	 |_creeaza o structura carteziana (grid) 2D, periodica

Functii MPI necesare: MPI_Comm_size(), MPI_Comm_rank(), MPI_Dims_create(), MPI_Cart_create()
-----------------------------------------------------------------------------------------------
	  _
	 | Procesul 0 (master):
	 |    aloca o matrice de intregi de dimensiune XSIZE x YSIZE
	 |    initializeaza toate elementele cu valoarea 0.75*MAXGREY
Pasul 2 -| Celelalte procese (worker):
	 |    aloca o matrice de dimensiune XSIZE/nx x YSIZE/ny
	 |_   initializeaza toate elementele cu valoarea 0.75*MAXGREY

XSIZE = YSIZE = 128, MAXGREY = 100, alocare dinamica
-----------------------------------------------------------------------------------------------
	  _
	 | afla coordonatele carteziene (x,y) ale fiecarui proces
	 | daca (x+y+1)mod2 == 1 : atribuie valoarea 0 (negru) elementelor din matricea locala	
Pasul 3 -| altfel : atribuie valoarea MAXGREY (alb) elementelor din matricea locala
	 |_creeaza tipuri de date derivate pentru transferul datelor catre procesul master

Functii MPI necesare: MPI_Cart_coords()
-----------------------------------------------------------------------------------------------
	  _
	 | Procesul 0 (master):
	 | 	itereaza de la 1 la numar de procese - 1
	 |	  receptioneaza datele folosind tipul derivat in zona corespunzatoare a matricii XSIZE x YSIZE
Pasul 4 -|	scrie fisierul de date pgm
	 | Celelalte procese (worker):
	 |_ 	transmite datele catre procesul master 

Datele sunt trimise bloc de procesele worker si receptionate de procesul master ca un tip derivat (atentie, si procesul master
proceseaza date!). In matricea (spatiul de adrese) globala de la procesul master, datele vor fi stocate la elementul 
GArray[dx*coord[0] + coord[1]*dy*XSIZE], dx=XSIZE/nx, dy=YSIZE/ny.
Functii MPI necesare: MPI_Type_vector(), MPI_Type_commit(), MPI_(I)Send(), MPI_Recv(), MPI_Wait().

	  Termina mediul MPI



 

*/

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

	MPI_Type_vector(Bsize, 1, )
}
