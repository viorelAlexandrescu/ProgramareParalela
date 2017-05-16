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

Etapa 1
-------

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



Etapa 2
-------

- Fiecare proces mentine doua blocuri: blocul local curent (GBlock) si blocul "evoluat" (GBlockNext) la fiecare iteratie
- Blocurile locale au (deltaX + 2)*(deltaY+2) pixeli - doua extra-linii si doua extra-coloane pentru "halo"
- Tipuri de date derivate:
	- pentru receptie la procesul 0:
	  MPI_Type_vector( deltaY, deltaX, XSIZE, MPI_INT, &block_t );
  	  MPI_Type_commit( &block_t );

	- pentru transmitere, procesele != 0
  	  MPI_Type_vector( deltaY, deltaX, deltaX + 2, MPI_INT, &sendblock_t );
          MPI_Type_commit( &sendblock_t );

	- coloane si linii, pentru MPI_Sendrecv():
          MPI_Type_vector( deltaY + 2, 1, deltaX + 2, MPI_INT, &col_t); 	// coloana pentru swap
  	  MPI_Type_commit( &col_t );

  	  MPI_Type_vector( deltaX, 1, 1 , MPI_INT, &line_t); 	// linia pentru swap cu cele doua extracoloane
          MPI_Type_commit( &line_t );

- Rangurile proceselor vecine in topologia carteziana definita? Pentru definitia regiunilor "halo"...
  MPI_Cart_shift( cartComm, 0, 1, &up, &down );
  MPI_Cart_shift( cartComm, 1, 1, &left, &right );

- Reguli de evolutie (0 <= nvec <= 8 este numarul vecinilor de ordin I negri ai pixelului curent):

     ******************************************************************************************************************
     * Identific numarul de vecini negri ("vii") ai fiecarui pixel din miezul blocului local continand starea curenta *
     * Regulile de evolutie (se aplica doar miezului!):                                                               *
     * - daca nvec < 2 si pixelul este negru, devine alb ("nu se poate reproduce")				      *
     * - daca nvec == 2 pixelul isi pastreaza culoarea                                                                *
     * - daca nvec >= 3 si nvec <= 7 si pixelul este alb, devine negru ("celula se naste") 			      *
     * - daca nvec > 7 si pixelul este negru, devine alb ("suprapopulare")					      *
     * ****************************************************************************************************************

- Itereaza regulile de evolutie de ITER ori; starea initiala a miezului la iteratia urmatoare este starea curenta a miezului acestei iteratii:
   ************************ PAS 4 ***************************************************
   * - daca proces cu rang != 0                                                     *
   *     transmite date brute catre procesul 0                                      *
   * - altfel                                                                       *
   *     itereaza de la procesul 1 la procesul nproc-1                              *
   *       primeste datele in regiunea corecta a buffer-ului, folosind tipul derivat*
   * - scrie datele in fisier (la fiecare 2 iteratii)                               *
   * - finalizeaza MPI                                                              *
   **********************************************************************************
*/

int main(int argc, char** argv) {

	int my_rank, nproc, source, dest = MASTER, tag = 1;
	int NX, NY, i, j, dims[NDIM], GSIZE = XSIZE*YSIZE;
	int reorder = 0, periods[NDIM];
	int deltaX, deltaY, Bsize;
	float *garray, *barray;
	int coords[NDIM];

	MPI_Comm comm = MPI_COMM_WORLD, cartcomm;
 	MPI_Status status;
	MPI_Datatype block_t;

	char fname[64];
/*
	  Initializare MPI
		  _
		 | determina numarul de procese lansate in executie
	Pasul 1	-| determina nx = numar de procese pe directia x in topologia virtuala
		 |           ny = numar de procese pe directia y in topologia virtuala
		 |_creeaza o structura carteziana (grid) 2D, periodica


*/
	for (i = 0; i < NDIM; i++) {
		periods[i] = TRUE;
		dims[i] = 0;
		coords[i] = 0;	
	}

	MPI_Init(&argc, &argv);
    	MPI_Comm_size(comm, &nproc);
    	MPI_Comm_rank(comm, &my_rank);

	//distributia optima a proceselor pe topologia virtuala 2d

	MPI_Dims_create(nproc, NDIM, dims);
	NX = dims[0];
	NY = dims[1];

	MPI_Cart_create(comm, NDIM, dims, periods, reorder, &cartcomm);

/*	argv
	 | Procesul 0 (master):
		 |    aloca o matrice de intregi de dimensiune XSIZE x YSIZE
		 |    initializeaza toate elementele cu valoarea 0.75*MAXGREY
	Pasul 2 -| Celelalte procese (worker):
		 |    aloca o matrice de dimensiune XSIZE/nx x YSIZE/ny
		 |_   initializeaza toate elementele cu valoarea 0.75*MAXGREY

	XSIZE = YSIZE = 128, MAXGREY = 100, alocare dinamica
*/
	if (my_rank == MASTER) {
		garray = (float *)malloc(GSIZE * sizeof(float));

		for (i = 0; i < GSIZE; i++) 	
			garray[i] = 0.75*MAXGREY;
	}

	Bsize = GSIZE/NX/NY; //dim matricii locale
	barray = (float *)malloc(Bsize * sizeof(float));
	
	for (i = 0; i < Bsize; i++) 	
		barray[i] = 0.75*MAXGREY;

/*		 
	| afla coordonatele carteziene (x,y) ale fiecarui proces
		 | daca (x+y+1)mod2 == 1 : atribuie valoarea 0 (negru) elementelor din matricea locala	
	Pasul 3 -| altfel : atribuie valoarea MAXGREY (alb) elementelor din matricea locala
		 |_creeaza tipuri de date derivate pentru transferul datelor catre procesul master

	Functii MPI necesare: MPI_Cart_coords()
*/

	MPI_Cart_coords(cartcomm, my_rank, NDIM, coords);

	for(i = 0; i < Bsize; i++) {
		if((coords[0]+coords[1]+1)%2 == 1)
			barray[i] = 0;
		else 
			barray[i] = MAXGREY;
	}

	deltaX = XSIZE/NX;
	deltaY = YSIZE/NY;

	MPI_Type_vector(deltaY, deltaX, XSIZE, MPI_FLOAT, &block_t);
	MPI_Type_commit(&block_t);

/*
		  _
	 | Procesul 0 (master):
	 | 	itereaza de la 1 la numar de procese - 1
	 |	  receptioneaza datele folosind tipul derivat in zona corespunzatoare a matricii XSIZE x YSIZE
Pasul 4 -|	scrie fisierul de date pgm
	 | Celelalte procese (worker):
	 |_ 	transmite datele catre procesul master

*/
	if(my_rank != MASTER) {
		MPI_Send(barray, Bsize, MPI_FLOAT, 0, tag, comm);
		free(barray);
	} else {
		for(i = 0; i < deltaY; i++)
			for(j = 0; j < deltaX; j++) {
				*(garray+i*XSIZE+j) = *(barray+i*deltaX+j);
			}

		for(i = 1; i < nproc; i++) {
			MPI_Cart_coords(cartcomm, i, NDIM, coords);
			MPI_Recv(garray+deltaX*coords[0]+coords[1]*deltaY*XSIZE, 1, block_t, i, tag, comm, &status);
		}
		pgm_write("poza.pgm", garray, XSIZE, YSIZE);
		free(barray);
		free(garray);
	}

	int crs;
	for(i = 0; i < (deltaY + 2); i++)
	 for(j = 0; j < (deltaX + 2); j++)
	  *(barray + i * (deltaX + 2) + j) = *(barrayNext + i * (deltaX + 2) j) = MAXGREY;

	if(coords[0] == NY/2)
	 for(j = 0; j < deltaX; j++)
	  *()
	MPI_Finalize();
	return 0;
}
