// INFORAMCIÓN /////////////////////////////////////////////////////////////////

/*\
 * TRABAJO PRÁCTICO TRANSVERSAL - TEORÍA DE LA COMPUTACIÓN I
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
 * - natsort: https://github.com/sourcefrog/natsort
\*/

// PRECOMPILADOR ///////////////////////////////////////////////////////////////

#define DEBUG 0

#include <stdio.h> // Entrada y salida estándar
#include <stdlib.h> // Librería general estándar
#include <string.h> // Manejo de cadenas estándar
#include <stdbool.h> // Tipo de dato booleano estándar

#include "include/LSS.h" // TAD "Lists, sets & strings"
#include "include/sds/sds.h" // Librería externa "Simple Dynamic Strings"

// DECLARACIONES ///////////////////////////////////////////////////////////////

// AF

/* NONE = Ninguno
 * AFND = Automata Finito No Determinista
 * AFD = Automata Finito Determinista */
enum AF_TYPE { NONE, AFND, AFD };
/* Q = Conjunto de estados
 * E = Conjunto de símbolos
 * D = Conjunto de transiciones
 * q0 = Estado inicial
 * F = Conjunto de estados de aceptación */
enum AF_ELEM { Q = 1, E, D, q0, F };
//
struct LSSNode *getTransition(struct LSSNode *delta, struct LSSNode *states, char *symbol);
//
char **splitStringBySymbols(char *string, struct LSSNode *symbols, int *size);
//
bool isStringAccepted(struct LSSNode *af, char *string);
//
enum AF_TYPE getAfType(struct LSSNode *af);

// Cadenas

// Recibe la entrada desde consola y se almacena dinámicamente
char *newUserString(void);
// Elimina el contenido de una cadena sin liberar la memoria asignada
void clearSds(char * str);

// Interfaz y otras

//
int mainMenu(enum AF_TYPE afType);
//
struct LSSNode *newAf(enum AF_TYPE *afType);
//
void printAf(struct LSSNode *automataFinito);
//
void acceptanceMenu(struct LSSNode *automataFinito);
//
int newUserOption(int min, int max);
// Ejecuta un comando para limpiar la consola
void clearConsole(void);
// Pide presionar ENTER para continuar
void pauseProgram(void);
// Elimina el buffer de stdin
void clearStdIn(void);

// MAIN ////////////////////////////////////////////////////////////////////////

int main(void) {
  if(DEBUG) printf("<i> Para desactivar mensajes del tipo \"<?>\" compile el "
  "programa sin \"-D DEBUG\"\n");

  struct LSSNode *automataFinito = NULL;
  enum AF_TYPE afType = NONE;
  int userOption;
  do { // Menu principal
    userOption = mainMenu(afType);
    switch(userOption) {
      case 1:
        freeLSS(&automataFinito);
        automataFinito = newAf(&afType);
        break;
      case 2:
        printAf(automataFinito);
        pauseProgram();
        break;
      case 3:
        acceptanceMenu(automataFinito);
        pauseProgram();
        break;
      case 4:
        break;
      case 0:
        break;
    }
  } while(userOption != 0);
  return 0;
}

// DEFINICIONES ////////////////////////////////////////////////////////////////

// AF

// Crea un array con los símbolos de la cadena original, sin modificarla
char **splitStringBySymbols(char *string, struct LSSNode *symbolsSet, int *arraySize) {
  // Si no hay símbolos
  if(!hasDataLS(symbolsSet)) {
    printf("<x> El conjunto de simbolos no contiene elementos\n");
    return NULL;
  }
  // Si la cadena no existe
  if(string == NULL) {
    printf("<x> La cadena a evaluar es NULL\n");
    return NULL;
  }
  // Array de cadenas
  char **array;
  // Cantidad de símbolos del AF
  int setSize = getSetCardinal(symbolsSet);
  // Cantidad de memoria asignada
  int memBlock = 2;
  // Posición en el array (y cantidad)
  int elem = 0;
  // Inicio de la subcadena (símbolo)
  int start = 0;
  // Caracter de la cadena principal
  int i = 0;
  // Simbolo del conjunto de símbolos del AF
  int j = 1;
  // Caracter de un simbolo del AF
  int k = 0;

  array = malloc(sizeof(*array) * memBlock);
  if(array == NULL) {
    printf("<x> Error en la asignacion de memoria dinamica\n");
    goto error;
  }
  // Caso de cadena NO vacía
  while(string[i] != '\0') {
    // Es necesario mas memoria?
    if(memBlock < elem + 1) {
      memBlock *= 2;
      char **aux = realloc(array, sizeof(*array) * memBlock);
      if(aux == NULL) {
        printf("<x> Error en la asignacion de memoria dinamica\n");
        goto error;
      }
      array = aux;
    }
    // Se obtiene el simbolo del conjunto
    char *afSymbol = getStringFromNode(getDataByPos(&symbolsSet, j));
    int symLenght = strlen(afSymbol);
    // Mientras los caracteres sean iguales
    while(afSymbol[k] == string[i] && afSymbol[k] != '\0' && string[i] != '\0') {
      k++; // Siguiente caracter del simbolo
      i++; // Siguiente caracter de la cadena
    }
    if(k == symLenght) {
      // Extraer simbolo
      array[elem] = sdsnewlen(string + start, symLenght);
      elem++;
      start = i;
      j = 1;
    } else {
      if(j == setSize) {
        printf("<x> La cadena tiene simbolos que no estan definidos en el AF\n");
        goto error;
      }
      j++; // Siguiente símbolo
    }
    i = start;
    k = 0;
  }
  // Caso de cadena "vacía"
  if(i == 0 && string[0] == '\0') {
    array[0] = sdsnew("");
    elem = 1;
  }

  *arraySize = elem;
  return array;

  error: // En caso de que algo falle se libera la memoria asignada
  for(j = 0; j < elem; j++) sdsfree(array[j]);
  free(array);
  *arraySize = 0;
  return NULL;
}

// Se asume que el AF es correcto (validar antes)
bool isStringAccepted(struct LSSNode *af, char *string) {

  // AF

  struct LSSNode *startState = getDataByPos(&af, q0);
  if(startState == NULL) return false;

  struct LSSNode *symbolSet = getDataByPos(&af, E);
  if(symbolSet == NULL) return false;

  struct LSSNode *deltaSet = getDataByPos(&af, D);
  if(deltaSet == NULL) return false;

  struct LSSNode *finalSet = getDataByPos(&af, F);
  if(finalSet == NULL) return false;

  // Cadena

  int numOfSymbols = 0;
  char **stringSymbols = splitStringBySymbols(string, symbolSet, &numOfSymbols);
  if(stringSymbols == NULL || numOfSymbols == 0) return false;

  // Transiciones

  // Estado inicial
  struct LSSNode *statesSet = newEmptySet();
  if(statesSet == NULL) {
    printf("<x> Error en la asignacion de memoria dinamica\n");
    return NULL;
  }
  if(!addStringToSet(&statesSet, getStringFromNode(startState))) {
    printf("<x> Error al insertar un elemento al conjunto\n");
    return NULL;
  }

  { // Las llaves limitan el alcance de la variable auxiliar
    struct LSSNode *aux = getTransition(deltaSet, statesSet, stringSymbols[0]);
    freeLSS(&statesSet);
    statesSet = aux;
  }

  // Por cada simbolo de la cadena
  for(int i = 1; i < numOfSymbols; i++) {
    statesSet = getTransition(deltaSet, statesSet, stringSymbols[i]);
  }

  // Es aceptada?

  struct LSSNode *intersection = newIntersectionSet(statesSet, finalSet);
  if(intersection == NULL) return false; // Error
  bool isAccepted = hasDataLS(intersection);

  // Liberacion de memoria

  sdsfreesplitres(stringSymbols, numOfSymbols);
  freeLSS(&intersection);

  // Resultado

  return isAccepted;
}

struct LSSNode *getTransition(struct LSSNode *delta, struct LSSNode *statesSet, char *symbol) {
  if(delta == NULL) {
    printf("<x> El conjunto de transiciones es NULL\n");
    return NULL;
  }
  if(statesSet == NULL) {
    printf("<x> El estado de partida es NULL\n");
    return NULL;
  }
  if(symbol == NULL) {
    printf("<x> El simbolo a utilizar es NULL\n");
    return NULL;
  }
  if(!hasDataLS(delta)) {
    printf("<x> El conjunto de transiciones no contiene elementos\n");
    return NULL;
  }

  struct LSSNode *targetStates = newEmptySet();
  if(targetStates == NULL) {
    printf("<x> Error en la asignacion de memoria dinamica\n");
    return NULL;
  }

  int statesSize = getSetCardinal(statesSet);
  int deltaSize = getSetCardinal(delta);

  // 1) CADENA VACIA
  if(strcmp(symbol, "") == 0) {
    for(int i = 1; i <= statesSize; i++) {
      if(!addStringToSet(&targetStates, getStringFromNode(getDataByPos(&statesSet, i)))) {
        printf("<x> Error al insertar un elemento en el conjunto\n");
        freeLSS(&targetStates);
        return NULL;
      }
    }
    return targetStates;
  }

  // 2) OTRAS CADENAS
  // Por cada transicion de Delta
  for(int i = 1; i <= deltaSize; i++) {
    struct LSSNode *transition = getDataByPos(&delta, i);
    char *fromState = getStringFromNode(getDataByPos(&transition, 1));
    char *tranSym = getStringFromNode(getDataByPos(&transition, 2));

    // Por cada estado de entrada
    for(int j = 1; j <= statesSize; j++) {
      char *state = getStringFromNode(getDataByPos(&statesSet, j));

      // Si simbolo y estado coinciden
      if(strcmp(symbol, tranSym) == 0 && strcmp(state, fromState) == 0) {
        struct LSSNode *toStates = getDataByPos(&transition, 3);
        int toStatesSize = getSetCardinal(toStates);

        // Por cada estado de llegada de la transicion
        for(int k = 1; k <= toStatesSize; k++) {
          if(!addStringToSet(&targetStates, getStringFromNode(getDataByPos(&toStates, k)))) {
            printf("<x> Error al insertar un elemento en el conjunto\n");
            freeLSS(&targetStates);
            return NULL;
          }
        }
      }
    }
  }
  return targetStates;
}

/* Condiciones para ser AFND
 * - Si en d, un estado tiene más de una transición con el mismo símbolo
 * - Si q0 es un conjunto
 * - Si el destino de una transicion tiene mas de un elemento */
enum AF_TYPE getAfType(struct LSSNode *af) {
  if(af == NULL) {
    printf("<x> El automata finito es NULL\n");
    return NONE;
  }
  if(getNodeType(af) != LST) {
    printf("<x> El automata finito no es valido\n");
    return NONE;
  }
  if(getListSize(af) < 5) {
    printf("<x> El automata finito no esta completo\n");
    return NONE;
  }

  struct LSSNode *startState = getDataByPos(&af, q0);
  if(startState == NULL) {
    printf("<x> El estado inicial es NULL\n");
    return NONE;
  }
  if(getNodeType(startState) != STR) return AFND;

  struct LSSNode *deltaSet = getDataByPos(&af, D);
  if(startState == NULL) {
    printf("<x> El conjunto de transiciones es NULL\n");
    return NONE;
  }
  int deltaSize = getSetCardinal(deltaSet);
  if(deltaSize < 0) {
    printf("<x> Error calculando la cantidad de transiciones definidas\n");
    return NONE;
  }

  char *fromStatePrev = "";
  char *symbolPrev = "";
  for(int i = 1; i <= deltaSize; i++) {

    struct LSSNode *transition = getDataByPos(&deltaSet, i);
    struct LSSNode *toStates = getDataByPos(&transition, 3);
    if(getSetCardinal(toStates) > 1) return AFND;

    char *fromState = getStringFromNode(getDataByPos(&transition, 1));
    char *symbol = getStringFromNode(getDataByPos(&transition, 2));
    if(i != 1) {
      if(strcmp(fromStatePrev, fromState) == 0 && strcmp(symbolPrev, symbol) == 0) {
        return AFND;
      }
      fromStatePrev = fromState;
      symbol = symbolPrev;
    }
  }
  return AFD;
}

// CADENAS

// Ingreso de una cadena de cualquier tamaño (asignación de memoria dinámica)
char *newUserString(void) {
  bool isAscii = true;
  size_t memoryBlock = 16;
  char * string;
  char * resultString;
  char character;
  size_t size = 0;

  string = realloc(NULL, sizeof * string * memoryBlock);
  if(!string) {
    printf("<x> Error en la asignacion dinamica de memoria\n");
    return string;
  }
  while(EOF != (character = fgetc(stdin)) && character != '\n') {
    string[size++] = character;
    if(size == memoryBlock) {
      memoryBlock *= 2;
      string = realloc(string, sizeof * string * (memoryBlock));
      if(!string) {
        printf("<x> Error en la asignacion dinamica de memoria");
        return string;
      }
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

void pauseProgram(void) {
  printf("Presione ENTER para continuar . . .");
  char c;
  // clearStdIn(); // No sé si quien llama esta función, ya limpió el buffer
  scanf("%c", &c);
  // clearStdIn(); // Limpio el buffer al finalizar el llamado

}

void clearConsole(void) {
  system("clear || cls");
}

void clearStdIn(void) {
  char c;
  while((c = getchar()) != '\n' && c != EOF);
}

int mainMenu(enum AF_TYPE afType) {
  int optionMin = 0;
  int optionMax = 1;
  if(afType == NONE) { // Primera iteración
    printf("<i> No hay un automata finito cargado\n");
    printf("1. Cargar automata finito\n");
  } else { // Resto de iteraciones
    if(afType == AFD) {
      printf("<i> El automata finito cargado es determinista\n");
    } else {
      printf("<i> El automata finito cargado es no determinista\n");
    }
    printf("1. Volver a cargar automata finito\n");
    printf("2. Mostrar el automata finito cargado\n");
    printf("3. Probar la aceptacion de una cadena en el automata finito\n");
    // Si el AF es un AFND
    if(afType == AFND) {
      printf("4. Convertir automata finito no determinista en determinista\n");
      optionMax = 4;
    } else optionMax = 3;
  }
  // Se muestra siempre
  printf("0. Salir\n");
  return newUserOption(optionMin, optionMax);
}

struct LSSNode *newAf(enum AF_TYPE *afType) {
  struct LSSNode *af;
  char *userString;

  printf("Carga de un automata finito\n");
  printf("Metodo de carga\n");
  printf("1. Por partes\n"); // Falta implementar la validacion
  printf("2. Directa\n"); // 

  switch(newUserOption(1, 2)) {
    case 1:
      printf("Carga por partes\n");
      af = newEmptyList();

      printf("Ingrese el conjunto de estados\n# ");
      userString = newUserString();
      // Verficiar que sea un conjunto de estados válido
      if(!appendStringToList(&af, userString)) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de simbolos del alfabeto\n# ");
      userString = newUserString();
      // Verificar que sea un conjunto de símbolos válido
      if(!appendStringToList(&af, userString)) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de transiciones\n# ");
      userString = newUserString();
      // Verificar que sea un conjunto de transiciones válido
      if(!appendStringToList(&af, userString)) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el estado inicial\n# ");
      userString = newUserString();
      // Verificar que sea un estado inicial válido
      if(!appendStringToList(&af, userString)) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de estados de aceptacion\n# ");
      userString = newUserString();
      // Verificar que sea un conjunto de estados de aceptación válido
      if(!appendStringToList(&af, userString)) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);
      break;

    case 2:
      printf("Carga directa\n");
      printf("Ingrese el automata finito completo\n");
      printf("# ");
      userString = newUserString();
      af = newLSFromString(userString);
      sdsfree(userString);
      break;
  }
  // Ver si automataFinito es determinista o no
  *afType = getAfType(af);
  return af;

  error:
  freeLSS(&af);
  sdsfree(userString);
  return NULL;
}

void printAf(struct LSSNode *af) {
  printf("Estados\n");
  printLS(getDataByPos(&af, Q));

  printf("\nSimbolos de entrada\n");
  printLS(getDataByPos(&af, E));

  printf("\nTransiciones\n");
  struct LSSNode *temp = getDataByPos(&af, D);
  int max = getSetCardinal(temp);
  for(int i = 1; i <= max; i++) {
    printLS(getDataByPos(&temp, i));
    printf("\n");
  }

  printf("Estado inicial\n");
  printLS(getDataByPos(&af, q0));
  printf("\nEstados de aceptacion\n");
  printLS(getDataByPos(&af, F));
  printf("\n");
}

void acceptanceMenu(struct LSSNode *automataFinito) {
  char *userString;
  printf("Ingrese la cadena a evaluar\n# ");
  userString = newUserString();
  if(isStringAccepted(automataFinito, userString)) {
    printf("La cadena \"%s\" SI ES ACEPTADA por el automata finito\n", userString);
  } else printf("La cadena \"%s\" NO ES ACEPTADA por el automata finito\n", userString);
  sdsfree(userString);
}
