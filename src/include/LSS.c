#include "LSS.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "auxlib.h"

////////////////////////////////////////////////////////////////////////////////
// DECLARACIONES PRIVADAS //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// VARIABLES-CONSTANTES ////////////////////////////////////////////////////////

// Codigos de error
enum ERR {
  // Error en asignación de memoria
  MALLOC,
  // Faltan llaves en la entrada
  NOT_SET,
  // Faltan corchetes en la entrada
  NOT_LST,
  // No es ni lista ni conjunto
  NOT_LS
};

// PROTOTIPOS //////////////////////////////////////////////////////////////////

// GENERAL

// Mensajes de error
void            errorMessages(enum ERR code, char *file, int line);
//
struct LSSNode *newLSFromStringRecursive(char * listOrSetAsString);
// 
struct LSSNode *newLSNode(enum NODE_TYPE typeToCreate);
//
void            printLSRecursive(struct LSSNode *listOrSet, bool isFirstElement);
//
void            linkNodeAsData(struct LSSNode *node, struct LSSNode *newDataNode);
//
void            linkNodeAsNext(struct LSSNode *node, struct LSSNode *newNextNode);
// 
struct LSSNode *getLastNode(struct LSSNode* listOrSet);
// Devuelve si la entrada es un conjunto, una lista o una cadena
enum NODE_TYPE  getStringType(char * string);
//
bool            isStringElementOfLS(struct LSSNode *listOrSet, char * elementAsString);
//
bool            isStringANumber(char * string);

// CONJUNTOS

void            addNodeInOrder(struct LSSNode *firstElement, struct LSSNode *newElement);

// LISTAS



// CADENAS

//
struct LSSNode *newStringNode(char * string);
// Extrae la cadena encerrada en llaves o corchetes
char          **splitString(char * string, int * sizeOfReturnedArray);
// Quita espacios alrededor de un elemento
void            deleteStringSpaces(char * string);
// Verifica que la entrada sea un conjunto o una lista
bool            isStringAListOrSet(char * string, bool isStringAnError);

////////////////////////////////////////////////////////////////////////////////
// DEFINICIONES PÚBLICAS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// GENERAL /////////////////////////////////////////////////////////////////////

// MARCA #1
struct LSSNode *newLSFromString(char *entrada) {
  // Validacion de la cadena de entrada
  if(!isStringAListOrSet(entrada, true)) return NULL;
  return newLSFromStringRecursive(entrada);;
}

void printLS(struct LSSNode * nodo) {
  // Comprobación de que exista al menos un nodo
  if(nodo == NULL) {
    printf("<i> No hay datos\n");
    return;
  }
  printLSRecursive(nodo, true);
  printf("\n");
}

void freeLSS(struct LSSNode ** sl) {
  // Exit si el nodo no existe
  if(*sl == NULL) return;
  if((*sl)->type != STR) {
    // Se libera el subconjunto o sublista
    freeLSS(&((*sl)->data));
    // Se libera el siguiente elemento
    freeLSS(&((*sl)->next));
  } else {
    // Se libera la cadena
    sdsfree((*sl)->str);
  }
  // Finalmente se libera el nodo actual
  free(*sl);
  *sl = NULL;
}

// CONJUNTOS ///////////////////////////////////////////////////////////////////

int getSetCardinal(struct LSSNode *nodo) {
  // Control conjunto vacío
  if(isEmptyLS(nodo)) return 0;

  int cnt = 0;
  while(nodo != NULL) {
    cnt++;
    nodo = nodo->next;
  }
  return cnt;
}

// LISTAS //////////////////////////////////////////////////////////////////////

struct LSSNode *newEmptyList() {
  struct LSSNode * nuevo = newLSNode(LST);
  return nuevo;
}

void appendStringToList(struct LSSNode ** lista, char * nuevo) {
  if((*lista)->type != LST) {
    // ERROR
    printf("<x> El primer argumento de \"%s\" no es una lista <%s:%d>\n", __func__, __FILE__, __LINE__);
    return;
  }
  struct LSSNode * data;
  if(isStringAListOrSet(nuevo, false)) {
    if(DEBUG) printf("<?> Insertando un conjunto o lista a la lista\n");
    data = newLSFromString(nuevo);
  } else {
    if(DEBUG) printf("<?> Insertando una cadena a la lista\n");
    data = newStringNode(nuevo);
  }
  if(!isEmptyLS(*lista)) linkNodeAsNext(getLastNode(*lista), newLSNode(LST));
  linkNodeAsData(getLastNode(*lista), data);
}

////////////////////////////////////////////////////////////////////////////////
// DEFINICIONES PRIVADAS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// GENERAL /////////////////////////////////////////////////////////////////////

void errorMessages(enum ERR key, char * f, int l) {
  switch(key) {
    case MALLOC:
      printf("<x> No se pudo reservar la memoria necesaria <%s:%d>\n", f, l);
      break;
    case NOT_SET:
      printf("<x> Falta una o mas llaves <%s:%d>\n", f, l);
      break;
    case NOT_LST:
      printf("<x> Falta uno o mas corchetes <%s:%d>\n", f, l);
      break;
    case NOT_LS:
      printf("<x> La entrada no es un conjunto ({ }) ni una lista ([ ]) <%s:%d>\n", f, l);
      break;
    default:
      printf("<x> Codigo de error desconocido <%s:%d>\n", f, l);
      break;
  }
}

struct LSSNode *newLSFromStringRecursive(char *string) {
  // Crear raiz
  struct LSSNode *rootNode = NULL;
  // De qué tipo es string
  enum NODE_TYPE stringType = getStringType(string);

  char **elementsArray; int elementsCount = 0;
  // Crear un array con los elementos de string
  elementsArray = splitString(string, &elementsCount);

  if(elementsCount > 0) { // Hay elementos
    for(int index = 0; index < elementsCount; index++) { // Por cada elemento
      struct LSSNode *newNode = newLSNode(stringType); // Nodo para el nuevo elemento, todos del mismo tipo
      if(getStringType(elementsArray[index]) != STR) { // El elemento es un subconjunto/sublista
        // Crear el subset/sublst y unirlo como elemento del actual (como dato)
        linkNodeAsData(newNode, newLSFromStringRecursive(elementsArray[index]));
      } else { // El elemento es STR
        // Si la cadena no es vacía, crear el nodo STR y unirlo como elemento (como dato)
        if(sdscmp(elementsArray[index], "") != 0) linkNodeAsData(newNode, newStringNode(elementsArray[index]));
      }
      if(rootNode == NULL) rootNode = newNode; // Si raiz aún no apunta a un nodo, inicializar
      else {
        linkNodeAsNext(getLastNode(rootNode), newNode);

        /* [Inserción ordenada - Aún no implementado]
        if(stringType != SET) {
          // Enlazar newNode como el ultimo elemento del arbol
          linkNodeAsNext(getLastNode(rootNode), newNode);
        } else {
          // Insertar el nuevo nodo de forma ordenada
          addNodeInOrder(rootNode, newNode);
        }
        */
      }
    } // Fin del ciclo
  } else { // No hay elementos
    // ERROR: Cantidad de elementos = 0 ???
  }
  sdsfreesplitres(elementsArray, elementsCount); // Libera la memoria utilizada por el array
  return rootNode;
}

struct LSSNode *newLSNode(enum NODE_TYPE type) {
  struct LSSNode *nuevo;
  nuevo = malloc(sizeof * nuevo);
  nuevo->type = type;
  nuevo->data = NULL;
  nuevo->next = NULL;
  return nuevo;
}

struct LSSNode *newStringNode(char *str) {
  struct LSSNode *nuevo;
  // nuevo = (struct LSSNode *)malloc(sizeof(struct LSSNode));
  nuevo = malloc(sizeof * nuevo);
  nuevo->type = STR;
  nuevo->str = sdsnew(str);
  return nuevo;
}

void linkNodeAsData(struct LSSNode *parent, struct LSSNode *data) {
  (*parent).data = data;
}

void linkNodeAsNext(struct LSSNode *parent, struct LSSNode *next) {
  parent->next = next;
}

struct LSSNode *getLastNode(struct LSSNode* primero) {
  if(primero->next != NULL) {
    return getLastNode(primero->next);
  } else return primero;
}

void deleteStringSpaces(char *str) {
  sdstrim(str, " ");
}

bool isStringAListOrSet(char *str, bool isStringAnError) {
  enum NODE_TYPE stringType = getStringType(str);
  size_t len = sdslen(str);
  bool isListOrSet = true; // Variable que se retornará
  bool isFirstTime = true;
  int *countPointer = NULL;
  int setCount = 0;
  int listCount = 0;
  // Control primer caracter
  switch(stringType) {
    case LST:
      listCount = 1;
      countPointer = &listCount;
      break;
    case SET:
      setCount = 1;
      countPointer = &setCount;
      break;
    case STR:
      if(isStringAnError) errorMessages(NOT_LS, __FILE__, __LINE__);
      return false; // Se omite el resto del análisis
      break;
  }
  // Control balance de {} y []
  for(int i = 1; i < len; i++) {
    switch(str[i]) {
      case '{': setCount++; break;
      case '}': setCount--; break;
      case '[': listCount++; break;
      case ']': listCount--; break;
    }
    if(*countPointer == 0 && isFirstTime) {
      if(i == len - 1) {
        // Si es el último caracter, entonces es un conjunto/lista
        isListOrSet = true;
      } else { // Si no es el último, el conjunto "cerró" antes del final y solo es un elemento de str
        isListOrSet = false;// El char de cierre no está al final
        if(isStringAnError) errorMessages(NOT_LS, __FILE__, __LINE__);
      }
      // Se impide cambiar el valor de isListOrSet nuevamente
      isFirstTime = false;
    }
  }
  // Si falta un corchete o una llave, se considera no válido
  if(setCount != 0) {
    errorMessages(NOT_SET, __FILE__, __LINE__);
    isListOrSet = false;
  }
  if(listCount != 0) {
    errorMessages(NOT_LST, __FILE__, __LINE__);
    isListOrSet = false;
  }
  return isListOrSet;
}

enum NODE_TYPE getStringType(char *str) {
  switch(str[0]) {
    case '{':
      if(DEBUG) printf("<?> Tipo \"conjunto\" detectado ('{')\n");
      return SET;
    case '[':
      if(DEBUG) printf("<?> Tipo \"lista\" detectado ('[')\n");
      return LST;
    default:
      if(DEBUG) printf("<?> Tipo \"cadena\" detectado ('%c')\n", str[0]);
      return STR;
  }
}

/*
/ Retorna true si str es un conjunto o una cadena, comprobando si el ultimo
 * caracter es el cierre del primero, usando stack /
bool isStringValid(char *str) {
  int openLSCount1 = 0, openLSCount2; // Se crea el contador
  char openSymbol1, closeSymbol1; // Caracter de apertura y cierre
  bool isListOrSet = true;
  switch(getStringType(str)) {
    case STR: // No es conjunto ni lista
      errorMessages(NOT_LS, __FILE__, __LINE__);
      return false;
      break;
      // Es conjunto, se usan los caracteres correspondientes
    case SET: openSymbol1 = '{'; closeSymbol1 = '}'; break;
      // Es lista, se usan los caracteres correspondientes
    case LST: openSymbol1 = '['; closeSymbol1 = ']'; break;
  }
  // Se analiza caracter por caracter
  for(int i = 0; i < sdslen(str); i++) {
    // Si el caracter es el de apertura, se incrementa el contador
    if(str[i] == openSymbol1) openLSCount1++;
    // Si el caracter es el de cierre:
    else if(str[i] == closeSymbol1) {
      // Se decrementa el contador
      openLSCount1--;
      // Si el contador es cero:
      if(openLSCount1 == 0) {
        // Si es el último caracter, entonces es un conjunto/lista
        if(i == sdslen(str) - 1) return true; // El char de cierre esta al final
        // Si no es el último, el conjunto "cerró" antes del final y solo es un elemento de str
        else { // El char de cierre no está al final
          errorMessages(NOT_LS, __FILE__, __LINE__);
          return false;
        }
      }
    }
  }
  return false; // ERROR: Faltan llaves o corchetes
}
*/

// Separa str en un array donde cada posición es un elemento
char **splitString(char *str, int *cant) {
  char **array; // Array
  int lar = sdslen(str); // Largo de la cadena str
  int elem = 0; // Cantidad de elementos del array
  int mem = 2; // Cantidad de "espacios" que se añaden al asignar memoria
  int ini = 0; // Inicio de cada elemento de str
  int i = 0; // Posición del caracter de str siendo analizado

  int n_set = 0; // Contador de conjuntos "abiertos"
  int n_lst = 0; // Contador de listas "abiertas"

  // Se decide cómo proceder
  int req_set = 0; // Suponiendo que str no es un conjunto
  int req_lst = 0; // Suponiendo que str no es una lista

  if(isStringAListOrSet(str, true)) { // Se confirma que str sí es un conjunto o una lista
    ini = 1; // Para ignorar el caracter de apertura
    lar = lar - 1; // Para ignorar el caracter de cierre
    switch(getStringType(str)) {
      // n_set debe ser 1 y n_lst debe ser 0 para tener en cuenta una coma
      case SET: req_set = 1; req_lst = 0; break;
        // n_set debe ser 0 y n_lst debe ser 1 para tener en cuenta una coma
      case LST: req_set = 0; req_lst = 1; break;
        // Caso inalcanzable debido a "isStringAListOrSet" (?)
      case STR: break; // ERROR
    }
  }

  // Control del largo invalido
  if(lar < 0) return NULL;
  // Asignación de memoria inicial
  //array = malloc(sizeof(char *) * mem);
  array = malloc(sizeof * array * mem);
  if(array == NULL) return NULL; // ERROR en la asignación
  // Caso en el que la cadena no tiene caracteres
  if(lar == 0) { *cant = 0; return array; }

  // Mientras no sea el fin de la cadena
  while(str[i] != '\0') {
    // Asignación de memoria extra si es necesario
    if(mem < elem + 2) {
      mem *= 2;
      char * *aux = realloc(array, sizeof * array * mem);
      if(aux == NULL) goto error; // ERROR en la asignación
      array = aux;
    }
    // Control de llaves y corchetes
    switch(str[i]) {
      case '{': n_set++; break; // Se "ingresa" a un conjunto
      case '}': n_set--; break; // Se "sale" de un conjunto
      case '[': n_lst++; break; // Se "ingresa" a una lista
      case ']': n_lst--; break; // Se "sale" de una lista
    }
    // Revisar comas solo si no se está "dentro" de un elemento
    if(n_lst == req_lst && n_set == req_set) {
      // Si hay una coma, separar todo lo anterior
      if(str[i] == ',') {
        array[elem] = sdsnewlen(str + ini, i - ini);
        if(array[elem] == NULL) goto error; // ERROR en la asignación
        elem++; // Se incrementa la cantidad de elementos del array
        ini = i + 1; // Se actualiza en dónde empieza "lo anterior"
      }
    }
    i++; // Para analizar el siguiente caracter de str
  }
  if(n_lst != 0) {
    // ERROR: Faltan corchetes (innecesario ???)
    goto error;
  }
  if(n_set != 0) {
    // ERROR: Faltan llaves (innecesario ???)
    goto error;
  }
  // Se añade lo que haya después de la última coma o el único elemento
  array[elem] = sdsnewlen(str + ini, lar - ini);
  if(array[elem] == NULL) goto error; // ERROR en la asignación
  elem++;
  *cant = elem; // Se modifica el parámetro a retornar
  return array; // Se retorna el array

  error: // En caso de que algo falle luego de asignar memoria dinámica
  for(int i = 0; i < elem; i++) sdsfree(array[i]); // Se liberan las char *
  free(array); // Se libera el array
  *cant = 0;
  return NULL;
}
void printLSRecursive(struct LSSNode * nodo, bool primero) {
  // Caso del conjunto vacío o lista vacía
  if(nodo == NULL) {
    printf(" ");
    return;
  }
  enum NODE_TYPE type = nodo->type;
  if(type == STR) {
    // Se muestra la cadena
    printf("%s", nodo->str);
    return;
  }
  // Se abre el conjunto o lista
  if(primero) (type == SET) ? printf("{") : printf("[");
  // Se muestra el dato
  printLSRecursive(nodo->data, true);
  if(nodo->next == NULL) {
    // Se cierra el conjunto o lista
    type == SET ? printf("}") : printf("]");
    return;
  }
  // Se muestra el siguiente elemento
  printf(",");
  printLSRecursive(nodo->next, false);
}

bool isEmptyLS(struct LSSNode * sl) {
  if(sl->data != NULL) return false;
  else {
    if(DEBUG) printf("<?> Conjunto vacio detectado\n");
    return true;
  }
}

bool isStringANumber(char * string) {
  int index = 0;
  int lenght = sdslen(string);
  bool isNumber = true;
  while(index < lenght && isNumber == true) {
    if(string[index] < '0' || string[index] > '9') isNumber = false;
  }
  return isNumber;
}

// CONJUNTOS ///////////////////////////////////////////////////////////////////

int getDataPriority(struct LSSNode * data) {
  if(data == NULL) return 0; // El nodo existe
  int dataType;
  if(data->type == STR) dataType = isStringANumber(data->str) ? 1 : 2;
  else dataType = (data->type == SET) ? 3 : 4;
  return dataType;
}



// 0 - Primero el vacío
// 1 - Si es numero, primero el menor
// 2 - Si no es numero, en orden alfabético
// 3 - Si no es cadena, primero conjuntos y luego listas
// 4 - Si es un conjunto o una lista, primero el que tenga menos elementos
// En cualquier caso, si son iguales, no insertar
void addNodeInOrder(struct LSSNode *currentElement, struct LSSNode *new) {
  if(currentElement != NULL) {
    // El nodo existe
    struct LSSNode *previousElement = NULL;
    int priorityNew = getDataPriority(new->data);
    bool hasNext = true;
    while(priorityNew > getDataPriority(currentElement->data) && hasNext) {
      if(currentElement->next == NULL) hasNext = false;
      else {
        previousElement = currentElement;
        currentElement = currentElement->next;
      }
    }
    if(!hasNext) { // Si se llego al final y no hay mas elementos
      // no se encontraron elementos que deban ir después del nuevo por lo tanto va al final
      linkNodeAsNext(previousElement, new);
    } else {
      switch(priorityNew) {
        case 0: // No tiene datos
          if(priorityNew <= getDataPriority(currentElement)) {

          }
          break;
        case 1: // Es un número
          break;
        case 2: // Es una cadena
          break;
        case 3: // Es un conjunto
          break;
        case 4: // Es una lista
          break;
      }
    }
  } else {
    // El nodo no existe
    // ERROR, al menos tiene que ser un conjunto vacío
  }
}

// LISTAS //////////////////////////////////////////////////////////////////////



// CADENAS /////////////////////////////////////////////////////////////////////


