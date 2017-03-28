#ifndef _GPLOT_H_
#define _GPLOT_H_
// insert definitions from this point forth
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CMD_SIZE 1024
/*
 Structura necesara comenzi unei aplicatii externa de grafice (GNU Plot)
 2 campuri: Stream-ul de comenzi 
	    si stilul de reprezentare grafica 
*/
typedef struct _GP_CMD_ {
   FILE *gpcmd; //stream for pipe commands
   char pstyle[32]; //plot representation type
}gp_cmd;


/*
*******************************************************************************************
Nume functie: check_X_display
Input: void
Return: int (0 - mediul X este activ |  1 - mediul X este inactiv)
Rol: Testeaza daca server-ul X Window este activ verificand variabila DISPLAY
*******************************************************************************************
*/
int check_X_display(void);

/*
*******************************************************************************************
Nume functie: gp_init
Input: void
Return: Stream de date folosit pentru comunicare cu GNU Plot
Rol: Init gnu plot
*******************************************************************************************
*/
gp_cmd *gp_init(void);


/*
*******************************************************************************************
Nume functie: gp_close
Input: Handle pe o sesiune gnuplot activa
Return: void
Rol: Inchide o sesiune gnuplot activa
*******************************************************************************************
*/
void gp_close(gp_cmd *handle);


/*
*******************************************************************************************
Nume functie: gp_command
Input: Lista variabila de argumente incluzand un handle pe sesiunea gnuplot activa, si
comenzi gnuplot
Return: void
Rol: Comunicare cu subprocesului gnuplot
Observatie: Verificare valabilitatii sintaxei gnuplot este in sarcina utilizatorului
*******************************************************************************************
*/
void gp_command(gp_cmd *handle,char* command,...);

#endif
