#include "auxlib.h"
#include <stdlib.h>
#include <stdio.h>

// LINUX ///////////////////////////////////////////////////////////////////////

/*
#ifdef __unix__
void clearStdIn() { __fpurge(stdin); }
#endif
*/

// WINDOWS /////////////////////////////////////////////////////////////////////

/*
#ifdef _WIN32
void clearStdIn() { fflush(stdin); }
#endif
*/

// AMBOS ///////////////////////////////////////////////////////////////////////

void pauseProgram() {
  printf("Presione ENTER para continuar . . . \n");
  char c;
  // clearStdIn(); // No sé si quien llama esta función, ya limpió el buffer
  scanf("%c", &c);
  // clearStdIn(); // Limpio el buffer al finalizar el llamado
  
}

void clearConsole() {
  system("clear || cls");
}

void clearStdIn(){
  char c;
  while((c = getchar()) != '\n' && c != EOF);
}
