// INFORAMCIÓN /////////////////////////////////////////////////////////////////

/*\
 * TRABAJO PRÁCTICO TRANSVERSAL - TEORÍA DE LA COMPUTACIÓN I
 *
 * @ Ezequiel Lizandro Dzioba
 *
 * PARTE 1
 * 1) Cargar un autómata finito "por partes": primero el conjunto de estados,
 * luego el alfabeto y así (la transición de estados cargarla por terna).
 * 2) Crear un árbol de que almacene al autómata finito ingresado.
 * 3) Mostrar el autómata finito.
 * 
 * NOTAS
 * - No usar caracteres que no formen parte de ASCII estándar
 *
 * REFERENCIA
 * <x> Error
 * <!> Advertencia
 * <i> Información
 * <?> Debug
 *  #  Input
 *
 * FUENTES
 * - Casteo al usar malloc: https://stackoverflow.com/a/605858
 * - SDS Simple Dynamic Strings: https://github.com/antirez/sds
 * -
 *
\*/

// PRECOMPILADOR ///////////////////////////////////////////////////////////////

#include <stdio.h> // Entrada y salida estándar
#include <stdlib.h> // Librería general estándar
#include <string.h> // Manejo de cadenas estándar
#include <stdbool.h> // Tipo de dato booleano estándar

#include "include/auxlib.h" // Declaraciones y definiciones útiles
#include "include/LSS.h" // TAD Conjuntos y Listas
#include "include/sds/sds.h" // Librería externa "Simple Dynamic Strings"

// DECLARACIONES ///////////////////////////////////////////////////////////////

// Cadenas

// Recibe la entrada desde consola y se almacena dinámicamente
char *newUserString();
// Elimina el contenido de una cadena sin liberar la memoria asignada
void clearSds(char * str);

// Interfaz

int newUserOption(int, int);

// MAIN ////////////////////////////////////////////////////////////////////////

int main() {
  struct LSSNode *automataFinito;
  char * userString;
  int userOption;

  printf("<i> Para desactivar mensajes \"<?>\" defina DEBUG 0 <include/auxlib.h>\n");
  printf("Carga de un automata finito\n");
  printf("Metodo de carga\n");
  printf("1. Por partes\n");
  printf("2. Todo de una vez (test)\n");

  userOption = newUserOption(1, 2);

  switch(userOption) {
    case 1:
      printf("Carga por partes\n# ");
      automataFinito = newEmptyList();

      printf("Ingrese el conjunto de estados\n# ");
      userString = newUserString();
      appendStringToList(&automataFinito, userString);
      sdsfree(userString);

      printf("Ingrese el conjunto de simbolos del alfabeto\n# ");
      userString = newUserString();
      appendStringToList(&automataFinito, userString);
      sdsfree(userString);

      printf("Ingrese el conjunto de transiciones\n# ");
      userString = newUserString();
      appendStringToList(&automataFinito, userString);
      sdsfree(userString);

      printf("Ingrese el estado inicial\n# ");
      userString = newUserString();
      appendStringToList(&automataFinito, userString);
      sdsfree(userString);

      printf("Ingrese el conjunto de estados de aceptacion\n# ");
      userString = newUserString();
      appendStringToList(&automataFinito, userString);
      sdsfree(userString);
      break;
    case 2:

      printf("Carga directa\n");
      printf("Ingrese el automata finito completo\n");
      printf("# ");
      userString = newUserString();
      automataFinito = newLSFromString(userString);
      // printf("Entrada (test): %s\n", userString);
      sdsfree(userString);
      break;
  }

  printf("Resultado\n");
  printLS(automataFinito);

  freeLSS(&automataFinito);

  pauseProgram();

  return 0;
}

// DEFINICIONES ////////////////////////////////////////////////////////////////

// CADENAS

// Ingreso de una cadena de cualquier tamaño (asignación de memoria dinámica)
char *newUserString() {
  bool isAscii = true;
  size_t memoryBlock = 16;
  char * string;
  char * resultString;
  char character;
  size_t size = 0;

  string = realloc(NULL, sizeof * string * memoryBlock);
  if(!string) return string; // ERROR
  while(EOF != (character = fgetc(stdin)) && character != '\n') {
    string[size++] = character;
    if(size == memoryBlock) {
      memoryBlock *= 2;
      string = realloc(string, sizeof * string * (memoryBlock));
      if(!string) return string; // ERROR
    }
    if(character < 32 || character > 126) isAscii = false;
  }
  string[size++] = '\0';
  if(!isAscii) printf("<!> Ha ingresado caracteres que pueden provocar errores\n");

  // Convierte a cadena compatible con las funciones de sds
  resultString = sdsnew(string);
  free(string);
  return resultString;
}

void clearSds(char *str) {
  str[0] = '\0';
  // Actualiza el tamaño de la sds a 0
  sdsupdatelen(str);
  // Elimina la memoria extra previamente asignada
  str = sdsRemoveFreeSpace(str);
}

// INTERFAZ

int newUserOption(int min, int max) {
  int x = -1;
  while(1) {
    printf("# ");
    scanf("%d", &x);
    clearStdIn();
    if(x < min || x > max) {
      printf("<!> Opcion invalida\n");
    } else return x;
  }
}