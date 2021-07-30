// INFORAMCIÓN /////////////////////////////////////////////////////////////////

/*\
 * TRABAJO PRÁCTICO TRANSVERSAL - TEORÍA DE LA COMPUTACIÓN I
 *
 * PARTE 1
 * 1) Cargar un autómata finito "por partes": primero el conjunto de estados,
 * luego el alfabeto y así (la transición de estados cargarla por terna).
 * 2) Crear un árbol que almacene al autómata finito ingresado.
 * 3) Mostrar el autómata finito.
 *
 * PARTE 2
 * 4) Dada una cadena, indicar si pertenece al lenguaje regular cuyo autómata
 * finito es el ingresado.
 * 5) Si el autómata finito es no determinista, convertirlo en determinista.
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
 * - SDS Simple Dynamic Strings: https://github.com/antirez/sds
 * - natsort: https://github.com/sourcefrog/natsort
\*/

// PRECOMPILADOR ///////////////////////////////////////////////////////////////

#include <stdio.h> // Entrada y salida estándar
#include <stdlib.h> // Librería general estándar
#include <string.h> // Manejo de cadenas estándar
#include <stdbool.h> // Tipo de dato booleano estándar

#include "include/LSS.h" // TAD "Lists, sets & strings"
#include "include/sds/sds.h" // Librería externa "Simple Dynamic Strings"

#ifndef DEBUG
#define DEBUG 0
#endif

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
/* Devuelve un conjunto con los estados alcanzados a partir de los estados del
 * conjuntoo 'states'.
 * 'delta' = conjunto de transiciones a utilizar
 * 'states' = conjunto de estados de partida
 * 'symbol' = símbolo de entrada */
struct LSSNode *getTransition(struct LSSNode *delta, struct LSSNode *states, char *symbol);
/* Divide una cadena, según los símbolos indicados, en elementos de un array. La
 * cadena original no se modifica.
 * 'string' = cadena a dividir
 * 'symbols' = conjunto de símbolos
 * 'size' = cantidad de elementos resultantes */
char **splitStringBySymbols(char *string, struct LSSNode *symbols, int *size);
/* Indica si una cadena es aceptada por un autómata finito */
bool isStringAccepted(struct LSSNode *af, char *string);
/* Validación simple de un autómata finito. Devuelve el tipo del mismo. */
enum AF_TYPE getAfType(struct LSSNode *af);
/* Convierte un autómata finito no determinista en uno determinista. No elimina
 * el original. */
struct LSSNode *afndToAfd(struct LSSNode *afnd);

// Cadenas

/* Recibe la entrada desde consola y se almacena dinámicamente. La memoria debe
 * ser liberada con 'sdsfree' */
char *newUserString(void);
/* Elimina el contenido de una cadena sin liberar su memoria asignada */
void clearSds(char * str);

// Interfaz y otras

/* Menú principal. */
int mainMenu(enum AF_TYPE afType);
/* Interfaz para ingresar un autómata finito */
struct LSSNode *newAf(void);
/* Interfaz para mostrar un autómata finito */
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
  if(DEBUG) printf("<i> Para desactivar mensajes del tipo \"<?>\" defina DEBUG 0\n");

  struct LSSNode *automataFinito = NULL;
  struct LSSNode *aux = NULL;
  enum AF_TYPE afType = NONE;
  int userOption;
  do { // Menu principal
    userOption = mainMenu(afType);
    switch(userOption) {
      case 1:
        freeLss(&automataFinito);
        automataFinito = newAf();
        afType = getAfType(automataFinito);
        break;
      case 2:
        printAf(automataFinito);
        pauseProgram();
        break;
      case 3:
        generateDotFile(automataFinito);
        break;
      case 4:
        acceptanceMenu(automataFinito);
        pauseProgram();
        break;
      case 5:
        aux = afndToAfd(automataFinito);
        if(aux != NULL) {
          automataFinito = aux;
          printf("La conversion ha sido realizada con exito\n");
        } else printf("La conversion no pudo ser realizada\n");
        afType = getAfType(automataFinito);
        pauseProgram();
        break;
      case 0:
        break;
    }
  } while(userOption != 0);
  freeLss(&automataFinito);
  return 0;
}

// DEFINICIONES ////////////////////////////////////////////////////////////////

// AF

char **splitStringBySymbols(char *string, struct LSSNode *symbolsSet, int *arraySize) {
  // Si no hay símbolos
  if(!hasDataLs(symbolsSet)) {
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
    if(memBlock < elem + 2) {
      memBlock *= 2;
      char **aux = realloc(array, sizeof(*array) * memBlock);
      if(aux == NULL) {
        printf("<x> Error en la asignacion de memoria dinamica\n");
        goto error;
      }
      array = aux;
    }
    // Se obtiene el simbolo del conjunto
    char *afSymbol = getElementString(getElementByPos(&symbolsSet, j));
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
  struct LSSNode *startState = getElementByPos(&af, q0);
  if(startState == NULL) return false;
  struct LSSNode *symbolSet = getElementByPos(&af, E);
  if(symbolSet == NULL) return false;
  struct LSSNode *deltaSet = getElementByPos(&af, D);
  if(deltaSet == NULL) return false;
  struct LSSNode *finalSet = getElementByPos(&af, F);
  if(finalSet == NULL) return false;

  struct LSSNode *statesSet = NULL;
  struct LSSNode *intersection = NULL;
  struct LSSNode *auxNode = NULL;
  int auxNum;

  //
  int numOfSymbols = 0;
  char **stringSymbols = splitStringBySymbols(string, symbolSet, &numOfSymbols);
  if(stringSymbols == NULL || numOfSymbols == 0) return false;

  if(DEBUG) {
    for(int a = 0; a < numOfSymbols; a++) printf("%s ", stringSymbols[a]);
    printf("\n");
  }

  statesSet = newEmptySet();
  if(statesSet == NULL) {
    printf("<x> Error en la asignacion de memoria dinamica\n");
    goto error;
  }

  // Estado inicial
  auxNode = cloneLss(startState, getElementType(startState));
  auxNum = addElementToSet(&statesSet, auxNode);
  if(auxNum <= 0) {
    if(auxNum < 0) {
      printf("<x> Error al insertar un elemento al conjunto\n");
      goto error;
    }
    freeLss(&auxNode);
  }

  if(DEBUG) { printf("E. Inicial = ");printLss(statesSet);printf("\n"); }

  auxNode = getTransition(deltaSet, statesSet, stringSymbols[0]);
  freeLss(&statesSet);
  statesSet = auxNode;

  if(DEBUG) { printf("0) Resultado = "); printLss(statesSet); printf("\n"); }

  // Por cada simbolo de la cadena
  for(int i = 1; i < numOfSymbols; i++) {
    auxNode = getTransition(deltaSet, statesSet, stringSymbols[i]);
    freeLss(&statesSet);
    statesSet = auxNode;

    if(DEBUG) { printf("%d) Resultado = ", i); printLss(statesSet); printf("\n"); }
  }

  intersection = newIntersectionSet(statesSet, finalSet);
  if(intersection == NULL) goto error;
  bool isAccepted = hasDataLs(intersection);

  if(DEBUG) { printf("finalSet = ");printLss(finalSet);printf("\n"); }
  if(DEBUG) { printf("Intersection = "); printLss(intersection); printf("\n"); }

  sdsfreesplitres(stringSymbols, numOfSymbols);
  freeLss(&intersection);
  freeLss(&statesSet);

  return isAccepted;

  error:
  sdsfreesplitres(stringSymbols, numOfSymbols);
  freeLss(&intersection);
  freeLss(&statesSet);
  return false;
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
  if(!hasDataLs(delta)) {
    printf("<x> El conjunto de transiciones no contiene elementos\n");
    return NULL;
  }

  struct LSSNode *targetStates = newEmptySet();
  if(targetStates == NULL) {
    printf("<x> Error en la asignacion de memoria dinamica\n");
    return NULL;
  }

  struct LSSNode *temp1 = NULL;
  struct LSSNode *temp2 = NULL;
  int statesSize = getSetCardinal(statesSet);
  int deltaSize = getSetCardinal(delta);

  // 1) CADENA VACIA
  if(strcmp(symbol, "") == 0) {

    if(DEBUG) printf("<?> Transicion con cadena vacia\n");

    for(int i = 1; i <= statesSize; i++) {
      temp1 = getElementByPos(&statesSet, i);
      temp2 = cloneLss(temp1, getElementType(temp1));

      int res = addElementToSet(&targetStates, temp2);

      if(res <= 0) { // -1: Error | 0: Repetido
        if(res < 0) {
          printf("<x> Error al insertar un elemento en el conjunto\n");
          freeLss(&targetStates);
          return NULL;
        }
        freeLss(&temp2);
      }
    }
    return targetStates;
  }

  // 2) OTRAS CADENAS
  // Por cada transicion de Delta
  for(int i = 1; i <= deltaSize; i++) {
    struct LSSNode *transition = getElementByPos(&delta, i);
    struct LSSNode *fromState = getElementByPos(&transition, 1);

    char *tranSym = getElementString(getElementByPos(&transition, 2));

    // Por cada estado de entrada
    for(int j = 1; j <= statesSize; j++) {
      struct LSSNode *state = getElementByPos(&statesSet, j);
      if(getElementType(fromState) == SET && getElementType(state) != SET) {
        state = statesSet;
      }

      if(DEBUG) { printf("<?> Comparacion: state = ");printLss(state);printf(" | fromState = ");printLss(fromState);printf("\n"); }

      // Si simbolo y estado coinciden
      if(strcmp(symbol, tranSym) == 0 && compareLss(state, fromState) == 0) {
        struct LSSNode *toStates = getElementByPos(&transition, 3);
        int toStatesSize = getSetCardinal(toStates);

        if(DEBUG) { printf("<?> Transicion encontrada = ");printLss(transition);printf("\n"); }

        // Por cada estado de llegada de la transicion
        for(int k = 1; k <= toStatesSize; k++) {
          temp1 = getElementByPos(&toStates, k);
          temp2 = cloneLss(temp1, getElementType(temp1));

          int res = addElementToSet(&targetStates, temp2);

          if(res <= 0) {
            if(res < 0) {
              printf("<x> Error al insertar un elemento en el conjunto\n");
              freeLss(&targetStates);
              return NULL;
            }
            freeLss(&temp2);
          }
        }
      }
    }
  }
  return targetStates;
}

struct LSSNode *afndToAfd(struct LSSNode *afnd) {
  // Lista que representa al AFD
  struct LSSNode *afd = NULL;
  // Lista de nuevos estados del AFD (luego se convierte a conjunto)
  struct LSSNode *newQ = NULL;
  // Conjunto de nuevas transiciones del AFD
  struct LSSNode *newDelta = NULL;
  // Conjunto de símbolos del AFD
  struct LSSNode *newSymbols = NULL;
  // Nuevo estado inicial del AFD
  struct LSSNode *newStart = NULL;
  // Conjunto de nuevos estados de aceptación
  struct LSSNode *newFinal = NULL;

  // Cada estado de newQ representado como SET
  struct LSSNode *stateSET = NULL;
  // Cada estado de newQ representado como STR
  struct LSSNode *stateSTR = NULL;
  // Cada nuevo estado que se va encontrando representado como SET
  struct LSSNode *newStateSET = NULL;
  // Cada nuevo estado que se va encontrando representado como STR
  struct LSSNode *newStateSTR = NULL;
  // Cada nueva transición que se va encontrando
  struct LSSNode *newTran = NULL;
  // Conjunto de estados de aceptación del AFND
  struct LSSNode *oldFinal = NULL;
  // Conjunto de transiciones del AFND
  struct LSSNode *oldDelta = NULL;

  // Variable auxiliar que puede ser usada en 'freeLss'
  struct LSSNode *aux = NULL;
  // Variable auxiliar que NO debe ser usada en 'freeLss'
  struct LSSNode *auxNF = NULL;

  // Simbolo
  char *symbol;
  // Cantidad de estados de newQ
  int numStates;
  // Cantidad de símbolos del AFD
  int numSymbols;
  // Posición del estado actual en newQ
  int i = 1;

  // Conjunto de transiciones del AFND
  oldDelta = getElementByPos(&afnd, D);
  if(oldDelta == NULL) goto error;

  // Conjunto de estados de aceptación del AFND
  oldFinal = getElementByPos(&afnd, F);
  if(oldFinal == NULL) goto error;

  // Se inicializa la lista que contendrá al AFD
  afd = newEmptyList();
  if(afd == NULL) goto error;

  // Nuevo conjunto de símbolos (se mantienen igual)
  auxNF = getElementByPos(&afnd, E);
  if(auxNF == NULL) goto error;
  newSymbols = cloneLss(auxNF, getElementType(auxNF));
  if(newSymbols == NULL) goto error;
  auxNF = NULL;

  numSymbols = getSetCardinal(newSymbols);

  // Nuevo estado inicial (se convierte en un conjunto)
  newStart = newEmptySet();
  if(newStart == NULL) goto error;
  auxNF = getElementByPos(&afnd, q0);
  if(auxNF == NULL) goto error;
  if(addElementToSet(&newStart, auxNF) < 0) goto error;
  auxNF = NULL;
  aux = cloneLss(newStart, STR);
  if(aux == NULL) goto error;
  freeLss(&newStart);
  newStart = aux;
  aux = NULL;

  if(DEBUG) { printf("newStart = "); printLss(newStart); printf("\n"); }

  // Se inicializa la lista de nuevos estados empezando por el nuevo estado inicial
  // Se usará una lista para evitar el ordenamiento automático y al final se
  // convertirá en un conjunto
  newQ = newEmptyList();
  if(newQ == NULL) goto error;
  aux = cloneLss(newStart, STR);
  if(aux == NULL) goto error;
  if(!appendElementToList(&newQ, aux)) goto error;
  aux = NULL;

  numStates = 1;

  // Se inicializa el nuevo conjunto de transiciones
  newDelta = newEmptySet();
  if(newDelta == NULL) goto error;

  // Se inicializa el nuevo conjunto de estados de aceptación
  newFinal = newEmptySet();
  if(newFinal == NULL) goto error;

  // Por cada nuevo estado
  while(i <= numStates) {
    stateSTR = getElementByPos(&newQ, i);
    stateSET = newLssFromString(getElementString(stateSTR));

    if(DEBUG) { printf("stateSET = "); printLss(stateSET); printf("\n"); }

    // Por cada símbolo
    for(int j = 1; j <= numSymbols; j++) {
      symbol = getElementString(getElementByPos(&newSymbols, j));

      if(DEBUG) printf("symbol = %s\n", symbol);

      // Transición
      newStateSET = getTransition(oldDelta, stateSET, symbol);
      newStateSTR = cloneLss(newStateSET, STR);

      if(DEBUG) { printf("newStateSTR = "); printLss(newStateSTR); printf("\n"); }

      // Nueva transición conseguida
      newTran = newEmptyList();
      appendElementToList(&newTran, cloneLss(stateSTR, STR));
      appendElementToList(&newTran, newLssFromString(symbol));

      aux = newEmptySet();
      if(addElementToSet(&aux, cloneLss(newStateSTR, STR)) < 0) goto error;
      appendElementToList(&newTran, cloneLss(aux, SET));
      freeLss(&aux);


      if(DEBUG) { printf("newTran = "); printLss(newTran); printf("\n"); }

      addElementToSet(&newDelta, newTran);

      if(DEBUG) { printf("newDelta = "); printLss(newDelta); printf("\n"); }

      if(hasDataLs(newStateSET)) {
        // El estado conseguido es realmente nuevo?
        if(getPosByElement(newQ, newStateSTR) == 0) {
          appendElementToList(&newQ, cloneLss(newStateSTR, STR));
          numStates++;
        }
      }

      if(DEBUG) { printf("newQ = "); printLss(newQ); printf("\n"); }

      freeLss(&newStateSET);
      freeLss(&newStateSTR);
    }
    // Estados de aceptación
    aux = newIntersectionSet(stateSET, oldFinal);

    if(DEBUG) { printf("Interseccion = "); printLss(aux); printf("\n"); }

    if(hasDataLs(aux)) {
      addElementToSet(&newFinal, cloneLss(stateSTR, STR));
    }

    if(DEBUG) { printf("newFinal = "); printLss(newFinal); printf("\n"); printf("\n"); }

    freeLss(&stateSET);
    freeLss(&aux);
    i++;
  }
  // Cambio de LST a SET
  aux = cloneLss(newQ, SET);
  if(aux == NULL) goto error;
  freeLss(&newQ);
  newQ = aux;

  appendElementToList(&afd, newQ);
  appendElementToList(&afd, newSymbols);
  appendElementToList(&afd, newDelta);
  appendElementToList(&afd, newStart);
  appendElementToList(&afd, newFinal);
  return afd;

  error:
  freeLss(&afd);
  freeLss(&newQ);
  freeLss(&newDelta);
  freeLss(&newSymbols);
  freeLss(&newStart);
  freeLss(&newFinal);
  freeLss(&stateSET);
  freeLss(&newStateSET);
  freeLss(&newTran);
  freeLss(&aux);
  return NULL;
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
  if(getElementType(af) != LST) {
    printf("<x> El automata finito no es valido\n");
    return NONE;
  }
  if(getListSize(af) < 5) {
    printf("<x> El automata finito no esta completo\n");
    return NONE;
  }

  struct LSSNode *startState = getElementByPos(&af, q0);
  if(startState == NULL) {
    printf("<x> El estado inicial es NULL\n");
    return NONE;
  }
  if(getElementType(startState) != STR) return AFND;

  struct LSSNode *deltaSet = getElementByPos(&af, D);
  if(startState == NULL) {
    printf("<x> El conjunto de transiciones es NULL\n");
    return NONE;
  }
  int deltaSize = getSetCardinal(deltaSet);

  char *fromStatePrev = "";
  char *symbolPrev = "";
  for(int i = 1; i <= deltaSize; i++) {

    struct LSSNode *transition = getElementByPos(&deltaSet, i);

    struct LSSNode *toStates = getElementByPos(&transition, 3);

    if(getElementType(toStates) != SET) {
      printf("<x> El tercer elemento de una transicion debe ser un conjunto\n");
      return NONE;
    }
    if(getSetCardinal(toStates) > 1) return AFND;

    struct LSSNode *fromState = getElementByPos(&transition, 1);
    if(getElementType(fromState) != STR) {
      printf("<x> El primer elemento de una transicion debe ser una cadena\n");
      return NONE;
    }
    char *fromStateString = getElementString(fromState);
    struct LSSNode *symbol = getElementByPos(&transition, 2);
    if(getElementType(fromState) != STR) {
      printf("<x> El segundo elemento de una transicion debe ser una cadena\n");
      return NONE;
    }
    char *symbolString = getElementString(symbol);
    if(i != 1) {
      if(strcmp(fromStatePrev, fromStateString) == 0 && strcmp(symbolPrev, symbolString) == 0) {
        return AFND;
      }
      fromStatePrev = fromStateString;
      symbolString = symbolPrev;
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
    printf("3. Generar grafico de la estructura cargada\n");
    printf("4. Probar la aceptacion de una cadena en el automata finito\n");
    // Si el AF es un AFND
    if(afType == AFND) {
      printf("5. Convertir automata finito no determinista en determinista\n");
      optionMax = 5;
    } else optionMax = 4;
  }
  // Se muestra siempre
  printf("0. Salir\n");
  return newUserOption(optionMin, optionMax);
}

struct LSSNode *newAf(void) {
  struct LSSNode *af = NULL;
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
      if(!appendElementToList(&af, newLssFromString(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de simbolos del alfabeto\n# ");
      userString = newUserString();
      // Verificar que sea un conjunto de símbolos válido
      if(!appendElementToList(&af, newLssFromString(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de transiciones\n# ");
      userString = newUserString();
      // Verificar que sea un conjunto de transiciones válido
      if(!appendElementToList(&af, newLssFromString(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el estado inicial\n# ");
      userString = newUserString();
      // Verificar que sea un estado inicial válido
      if(!appendElementToList(&af, newLssFromString(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de estados de aceptacion\n# ");
      userString = newUserString();
      // Verificar que sea un conjunto de estados de aceptación válido
      if(!appendElementToList(&af, newLssFromString(userString))) {
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
      af = newLssFromString(userString);
      sdsfree(userString);
      break;
  }
  return af;

  error:
  freeLss(&af);
  sdsfree(userString);
  return NULL;
}

void printAf(struct LSSNode *af) {
  printf("Estados\n");
  printLss(getElementByPos(&af, Q));

  printf("\nSimbolos de entrada\n");
  printLss(getElementByPos(&af, E));

  printf("\nTransiciones\n");
  struct LSSNode *temp = getElementByPos(&af, D);
  int max = getSetCardinal(temp);
  for(int i = 1; i <= max; i++) {
    printLss(getElementByPos(&temp, i));
    printf("\n");
  }

  printf("Estado inicial\n");
  printLss(getElementByPos(&af, q0));
  printf("\nEstados de aceptacion\n");
  printLss(getElementByPos(&af, F));
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
