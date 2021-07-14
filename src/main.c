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
 * FEEDBACK
 * 1) El almacenamiento se hace correctamente
 * 2) Por ahora se acepta el uso de otro TAD a pesar de no ser necesario, en
 * este caso en el main usas correctamente un puntero a char.
 * 3) El programa no controla la repetición de elementos dentro de un conjunto
 * 4) Entrega APROBADA
 *
 * PARTE 2
 * 4) Dada una cadena, indicar si pertenece al lenguaje regular cuyo autómata
 * finito es el ingresado.
 * 5) Si el autómata finito es no determinista, convertirlo en determinista.
 * 
 * FEEDBACK
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

// AF

bool isAfnd(struct LSSNode * af);

// Cadenas

// Recibe la entrada desde consola y se almacena dinámicamente
char *newUserString();
// Elimina el contenido de una cadena sin liberar la memoria asignada
void clearSds(char * str);

// Interfaz

int newUserOption(int min, int max);

// MAIN ////////////////////////////////////////////////////////////////////////

int main() {
  /* FT = First time
  */
  enum FLAG { FT, AFD, AFND };
  struct LSSNode *automataFinito = NULL;
  char * userString;
  int userOptions[2] = {};
  enum FLAG flag = FT;
  int optionMin = 0;
  int optionMax = 1;

  if(DEBUG) printf("<i> Para desactivar mensajes \"<?>\" defina DEBUG 0 <include/auxlib.h>\n");

  do { // Menu principal
    if(flag == FT) { // Primera iteración
      printf("<i> No hay un automata finito cargado\n");
      printf("1. Cargar automata finito\n");
    } else { // Resto de iteraciones
      if(flag == AFND) {
        printf("<i> El automata finito cargado es no determinista\n");
      } else {
        printf("<i> El automata finito cargado es determinista\n");
      }
      printf("1. Volver a cargar automata finito\n");
      printf("2. Mostrar el automata finito cargado\n");
      printf("3. Probar la aceptacion de una cadena en el automata finito\n");
      // Si el AF es un AFND
      if(flag == AFND) {
        printf("4. Convertir automata finito no determinista en determinista\n");
        optionMax = 4;
      } else optionMax = 3;
    }
    // Se muestra siempre
    printf("0. Salir\n");
    userOptions[0] = newUserOption(optionMin, optionMax);

    switch(userOptions[0]) {
      case 1:
        freeLSS(&automataFinito);
        printf("Carga de un automata finito\n");
        printf("Metodo de carga\n");
        printf("1. Por partes\n");
        printf("2. Todo de una vez (test)\n");
        userOptions[1] = newUserOption(1, 2);
        switch(userOptions[1]) {
          case 1:
            printf("Carga por partes\n# ");
            automataFinito = newEmptyList();

            printf("Ingrese el conjunto de estados\n# ");
            userString = newUserString();
            // Verficiar que sea un conjunto de estados válido
            appendStringToList(&automataFinito, userString);
            sdsfree(userString);

            printf("Ingrese el conjunto de simbolos del alfabeto\n# ");
            userString = newUserString();
            // Verificar que sea un conjunto de símbolos válido
            appendStringToList(&automataFinito, userString);
            sdsfree(userString);

            printf("Ingrese el conjunto de transiciones\n# ");
            userString = newUserString();
            // Verificar que sea un conjunto de transiciones válido
            appendStringToList(&automataFinito, userString);
            sdsfree(userString);

            printf("Ingrese el estado inicial\n# ");
            userString = newUserString();
            // Verificar que sea un estado inicial válido
            appendStringToList(&automataFinito, userString);
            sdsfree(userString);

            printf("Ingrese el conjunto de estados de aceptacion\n# ");
            userString = newUserString();
            // Verificar que sea un conjunto de estados de aceptación válido
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
        // Ver si automataFinito es determinista o no
        flag = AFND;
        break;
      case 2:
        printf("Resultado: ");
        printLS(automataFinito);
        printf("\n");
        //pauseProgram();
        break;
      case 3:
        break;
      case 4:
        break;
      case 0:
        break;
    }
  } while(userOptions[0] != 0);
  return 0;
}

// DEFINICIONES ////////////////////////////////////////////////////////////////

// AF

/* Condiciones para ser AFND
 * - Si un elemento de Q es un conjunto
 * - Si en d, un estado tiene más de una transición con el mismo símbolo
 * - Si q0 tiene más de un elemento */
bool isAfnd(struct LSSNode * af){
  bool returnValue = true;
  return returnValue;
}

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
