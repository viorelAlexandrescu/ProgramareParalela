/*
-----------------------Pointeri------------------------
Definitie: Un pointer este o variabila care retine adresa de memorie a unui alt obiect.
-transmiterea de date prin referinta
-alocarea dinamica de memorie
-intoarcerea a mai mult de o valoare dintr-o functie

Functii:
-double gsl_sf_bessel_J0(double x);
-int scanf(char* format_string, ...);
-void* malloc(size_t numbytes);
-void free(void* ptr);

double y = 12.3, z;
z = gsl_sf_bessel_J0(y);

Operatori:
- & (address of)
- * (value at address)

declarare pointer: tip_data * nume_var_ptr;
tip_data * nume_sir_ptr[25];
tip **pptr;

int a = 15;
int*p = &a;
*p = 22;

valoare Null - desemneaza valoarea unui pointer care nu pointeaza catre nimic


FILE *fp;
if((fp=fopen("file.dat","w")) == NULL)
{
    fprintf(stderr, "Nu pot crea fisierul file.dat. Disc plin! Ies...\n");
    exit(1);
}
...
fclose(fp);
fp=NULL;


------------------Aritmetica pointerilor-----------------
++, --, +, -

Aritmetica pointerilor are ca rezultat ceva ce depinde de tipul de baza al pointerului.

int a = 15, b;
int* p = &a, *q;
p++; 
q = p+2;
b = *p++; //operatorul de incrementare are prioritate mai mare
*/


//Problema 1

#include <stdio.h>
#include <stdlib.h>
// /usr/bin/gsl/.. -locatia fisierelor header legate de gsl
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_math.h>
#include "gplot.h"
#include <unistd.h>

#define NMAX 500

int main(int argc, char** argv)
{
    if(argc!=2)
    {
        printf("*****************************************************************************************************\n");
        printf("Utilizare: %s nume_fisier_date\n",argv[0]);
        printf("**Programul evalueaza valorile functiei bessel J0 pe un grid de 500 de puncte in intervalul 0-20 si \n scrie valorile rezultate in fisierul nume_fisier_date*\n");
        printf("*****************************************************************************************************\n\n");
        exit(1);
    }
    
    //***Bloc declaratii
    double *datax, *dataJ; //bufferele de date alocate dinamica
    int i; //cursor de bucle
    FILE *fp;
    //***
    
    //pot aloca bufferele?
    if((datax=(double*)malloc(NMAX * sizeof(double))) == NULL)
    {
        fprintf(stderr,"Nu se poate aloca memoria RAM pentru bufferul datax. Ies..\n");
        return 2; //exit(2); 
    }
    if((dataJ=(double*)malloc(NMAX * sizeof(double))) == NULL)
    {
        fprintf(stderr,"Nu se poate aloca memoria RAM pentru bufferul dataj. Ies..\n");
        return 2; //exit(2); 
    }
    
    //initializare date
    for(i=0; i<NMAX; i++)
    {
        double xi = ((double)i)*20. / ((double)NMAX);
        *(datax+i) = xi;
        *(dataJ+i) = gsl_sf_bessel_J0(xi);
    }
    
    //deschidem fisierul
    if((fp=fopen(argv[1],"w")) == NULL)
    {
        fprintf(stderr, "Nu pot crea fisierul file.dat. Disc plin! Ies...\n");
        return 3;
    }
    
    for(i=0; i<NMAX; i++)
    {
        fprintf(fp,"%.5lf\t%.5lf\n",*(datax+i),*(dataJ+i));
    }
    
    fclose(fp); fp = NULL;
    free(datax); datax = NULL;
    free(dataJ); dataJ = NULL;
    
    //initializare sesiune gnuplot
    gp_cmd* handle;
    handle = gp_init();
    if(handle==NULL)
    {
        printf("Nu pot initializa sesiunea gnuplot.Fisierul de date a fost creat\n\n");
        return 2; //cod eroare gnuplot
    }
    set_X_label(handle, "x(div.)");
    set_Y_label(handle,"J0(x)");
    set_graphic_style(handle,"lines");
    
    //comanda: plot "nume_fisier" using 1:2 with lines
    //               [ argv[1] ]                 [handle->pstyle]
    
    gp_command(handle,"plot \"%s\" using 1:2 with %s", argv[1], handle->pstyle);
    sleep(10); 
    
    gp_command(handle,"set term pdf");
    gp_command(handle,"set output %s","pozaJ0.pdf");
    gp_command(handle,"plot \"%s\" using 1:2 with %s", argv[1], handle->pstyle);
    gp_close(handle);
    return 0;
}//end of main

/*pentru compilare (dupa ce am facut fisierul makefile):
 make gplot
*/

/*pt plotare
 *gnuplot
 * plot 'nume_fisier_date' using 1:2 with lines
*/














