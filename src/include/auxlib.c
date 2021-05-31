#include "auxlib.h"
#include <stdlib.h>
#include <stdio.h>

// LINUX ///////////////////////////////////////////////////////////////////////

/*
#ifdef __unix__
void cl_stdin() { __fpurge(stdin); }
#endif
*/

// WINDOWS /////////////////////////////////////////////////////////////////////

/*
#ifdef _WIN32
void cl_stdin() { fflush(stdin); }
#endif
*/

// AMBOS ///////////////////////////////////////////////////////////////////////

void pausa() {
  printf("Presione ENTER para continuar . . . \n");
  char c;
  // cl_stdin(); // No sé si quien llama esta función, ya limpió el buffer
  scanf("%c", &c);
  // cl_stdin(); // Limpio el buffer al finalizar el llamado
  
}

void cl_consola() {
  system("clear || cls");
}

void cl_stdin(){
  char c;
  while((c = getchar()) != '\n' && c != EOF);
}
