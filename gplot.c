#include "gplot.h"
// Implementare check_X_display()

int check_X_display(void){
 int errorCode = 0; //function return value (O - OK, 1 - Fail)
 char *displayValue; // value returned by getenv
 
 displayValue = getenv("DISPLAY");
 if(displayValue == NULL){
   errorCode = 1;
   fprintf(stderr, "Undefined DISPLAY value! Is X Server Inactive?\n");
 }
 return errorCode;
}


//Implementare gp_init()
gp_cmd *gp_init(void){
 //testam ca server-ul X este activ
   gp_cmd *handle;
   if(check_X_display()) return NULL; 

   if( (handle = (gp_cmd*)malloc(sizeof(gp_cmd))) == NULL) {
     fprintf(stderr, "Cannot initialize gnuplot. Out of memory... :(\n");
     return NULL
    }

    if((handle->gpcmd = popen("gnuplot","w")) == NULL){
     fprintf(stderr,"Cannot start gnuplot as child process. Missing package?\n");
     free(handle); 
     return NULL; 
    }
   strcpy(handle->pstyle,"lines");
   return handle;
}//end of gp_init definition

void gp_close(gp_cmd *handle){
 if(check_X_display()) return;
 if(pclose(handle->gpcmd) == -1){
  fprintf(stderr,"Unknown Error for gnuplot. Child process unaccesable\n");
  free(handle);
  return;
 }
 free(handle);
 return;
}//end of gp_close

void gp_command(gp_cmd *handle, char *cmd, ...){
 char local_cmd[CMD_SIZE];
 va_list ap;//init variable arg list
 va_start(ap,cmd);
 vsprintf(local_cmd,cmd,ap);//put what is in cmd and ap in local_cmd
 va_end(ap);
 strcpy(local_cmd,"\n");
 fputs(local_cmd,handle->gpcmd);
 fflush(handle->gpcmd);
 return;
}//end of gp_command
