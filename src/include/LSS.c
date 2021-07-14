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
//
int             getSizeOfLS(struct LSSNode *listOrSet);

// NODOS SET

//
int            addNodeInOrder(struct LSSNode **firstElement, struct LSSNode *newElement);
//
int            getDataPriority(struct LSSNode * dataNode);

// NODOS LST



// NODOS STR

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
struct LSSNode *newLSFromString(char *input) {
  // Validacion de la cadena de entrada
  if(!isStringAListOrSet(input, true)) return NULL;
  // Retorna la creación recursiva
  return newLSFromStringRecursive(input);;
}

void printLS(struct LSSNode * node) {
  // Comprobación de que exista al menos un nodo
  if(node == NULL) {
    printf("No hay datos");
    return;
  }
  printLSRecursive(node, true);
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

struct LSSNode *getElementByPos(struct LSSNode **ls, int pos) {
  if(!*ls) return NULL;
  if((*ls)->type == STR) return NULL;
  // Para no modificar la variable fuera de la función
  struct LSSNode *currentElement = *ls;
  int currentPos = 1;
  // Ciclo que se detiene al llegar a pos, o al último elemento
  while(currentPos < pos && !currentElement) {
    currentElement = currentElement->next;
    currentPos++;
  }
  // Si existe el elemento, retornarlo
  if(currentElement) return currentElement->data;
  else return currentElement;
}

// CONJUNTOS ///////////////////////////////////////////////////////////////////

int getSetCardinal(struct LSSNode *node) {
  return getSizeOfLS(node);
}

// LISTAS //////////////////////////////////////////////////////////////////////

struct LSSNode *newEmptyList() {
  struct LSSNode * new = newLSNode(LST);
  return new;
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

int getListSize(struct LSSNode *list) {
  return getSizeOfLS(list);
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
      else if(stringType != SET) {
        // Enlazar newNode como el ultimo elemento del arbol
        linkNodeAsNext(getLastNode(rootNode), newNode);
      } else {
        // Insertar el nuevo nodo de forma ordenada solo cuando es elemento de un conjunto
        addNodeInOrder(&rootNode, newNode);
      }
    } // <- Fin del ciclo
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
  nuevo = malloc(sizeof * nuevo);
  nuevo->type = STR;
  // Eliminar espacios alrededor del elemento
  deleteStringSpaces(str);
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
  size_t len = strlen(str);
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
      if(DEBUG) printf("<?> Tipo SET detectado ('{')\n");
      return SET;
    case '[':
      if(DEBUG) printf("<?> Tipo LST detectado ('[')\n");
      return LST;
    default:
      if(DEBUG) printf("<?> Tipo STR detectado ('%c')\n", str[0]);
      return STR;
  }
}

// Separa str en un array donde cada posición es un elemento
char **splitString(char *str, int *cant) {
  char **array; // Array
  int lar = strlen(str); // Largo de la cadena str
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
  bool isDecimal = false;
  bool isNumber = true;
  int index = 0;
  // ASCII , - . / 0 1 2 3 4 5 6 7 8 9 :
  while(string[index] != '\0' && isNumber == true) {
    // Posee caracteres válidos?
    if(string[index] < '-' || string[index] > '9' || string[index] == '/') isNumber = false;
    // Solo puede haber un signo negativo y debe estar al inicio
    if(string[index] == '-' && index != 0) isNumber = false;
    // Solo puede haber un punto
    if(string[index] == '.') {
      if(!isDecimal) isDecimal = true;
      else isNumber = false;
    }
    // Para analizar el siguiente caracter
    index++;
  }
  return isNumber;
}

int getSizeOfLS(struct LSSNode *node) {
  // Control conjunto vacío
  if(isEmptyLS(node)) return 0;

  int cnt = 0;
  while(node != NULL) {
    cnt++;
    node = node->next;
  }
  return cnt;
}

//  1: node1 > node2
// -1: node1 < node2
//  0: lss1 = set2
int compareNodes(struct LSSNode *node1, struct LSSNode *node2) {
  int returnValue = 0;
  int type1 = getDataPriority((*node1).data);
  int type2 = getDataPriority((*node2).data);
  // Comparación de prioridades
  if(type1 > type2) returnValue = 1;
  else if(type1 < type2) returnValue = -1;
  // Comparación de números
  else if(type1 == 1) {
    int number1 = atof(node1->data->str);
    int number2 = atof(node2->data->str);
    if(number1 > number2) returnValue = 1;
    else if(number1 < number2) returnValue = -1;
    else if(node1->next != NULL && node2->next != NULL) returnValue = compareNodes(node1->next, node2->next);
    else if(node1->next != NULL && node2->next == NULL) returnValue = 1;
    else if(node1->next == NULL && node2->next != NULL) returnValue = -1;
    // Comparación de cadenas
  } else if(type1 == 2) {
    if(sdscmp(node1->data->str, node2->data->str) > 0) returnValue = 1;
    else if(sdscmp(node1->data->str, node2->data->str) < 0) returnValue = -1;
    else if(node1->next != NULL && node2->next != NULL) return compareNodes(node1->next, node2->next);
    else if(node1->next != NULL && node2->next == NULL) returnValue = 1;
    else if(node1->next == NULL && node2->next != NULL) returnValue = -1;
    // Comparación de conjuntos y listas
  } else if(type1 == 3 || type1 == 4) {
    int size1 = getSizeOfLS(node1->data);
    int size2 = getSizeOfLS(node2->data);
    if(size1 > size2) returnValue = 1;
    else if(size1 < size2) returnValue = -1;
    else {
      int resComp = compareNodes(node1->data, node2->data);
      if(resComp < 0) returnValue = 1;
      else if(resComp > 0) returnValue = -1;
      else if(node1->next != NULL) returnValue = compareNodes(node1->next, node2->next);
    }
  }
  if(DEBUG) printf("<?> El resultado de \"%s\" es %d\n", __func__, returnValue);
  return returnValue;
}

// CONJUNTOS ///////////////////////////////////////////////////////////////////

int getDataPriority(struct LSSNode * data) {
  if(data == NULL) return 0; // El nodo existe
  int dataType;
  if(data->type == STR) dataType = isStringANumber(data->str) ? 1 : 2;
  else dataType = (data->type == SET) ? 3 : 4;
  if(DEBUG) printf("<?> La funcion \"%s\" retorna %d\n", __func__, dataType);
  return dataType;
}

/* {1,a,{1,a,[b]}} {1,a,{1,a,[c]}}
   {             } {             } =
    ^               ^              =
      ^               ^            =
        {        }      {        } =
         ^               ^         =
           ^               ^       =
             [ ]             [ ]   =
              ^               ^    <
   {             } {             } <
*/

// SOLO USAR EN CONJUNTOS
// - Primero el vacío
// - Si es numero, primero el menor
// - Si no es numero, en orden alfabético
// - Si no es cadena, primero conjuntos y luego listas
// - Si es un conjunto o una lista, primero el que tenga menos elementos
// - Si tienen la misma cantidad de elementos, ordenar según sus elementos
// En cualquier caso, si son iguales, no insertar
int addNodeInOrder(struct LSSNode **firstNode, struct LSSNode *newNode) {
  if(*firstNode == NULL) {
    printf("<x> El primer argumento de \"%s\" debe ser un conjunto <%s:%d>\n", __func__, __FILE__, __LINE__);
    return 0;
  }
  struct LSSNode *currentNode = *firstNode;
  struct LSSNode *previousNode = *firstNode;
  int currentPos = 1;
  int cmpNodes = compareNodes(newNode, currentNode);
  // Búsqueda de la posición
  while(cmpNodes > 0 && currentNode->next != NULL) {
    if(DEBUG) printf("<?> \"%s\" es mayor que \"%s\"\n", newNode->data->str, currentNode->data->str);
    previousNode = currentNode;
    currentNode = currentNode->next;
    currentPos++;
    cmpNodes = compareNodes(newNode, currentNode);
  }
  if(cmpNodes == 0) {
    printf("<i> Eliminando elemento repetido \"");
    printLS(newNode->data); printf("\"\n");
    freeLSS(&newNode);
    return 0;
  }
  // Caso especial para la primera posicion (raiz)
  if(currentPos == 1) {
    if(cmpNodes < 0) {
      newNode->next = *firstNode;
      *firstNode = newNode;
    } else {
      newNode->next = (*firstNode)->next;
      (*firstNode)->next = newNode;
      currentPos++;
    }
  } else if(cmpNodes < 0) {
    newNode->next = currentNode;
    previousNode->next = newNode;
  } else {
    newNode->next = currentNode->next;
    currentNode->next = newNode;
    currentPos++;
  }
  if(DEBUG) {
    printf("<?> Elemento \"");
    printLS(newNode->data);
    printf("\" insertado en la posicion %d\n", currentPos);
  }
  return currentPos;
}

// LISTAS //////////////////////////////////////////////////////////////////////



// CADENAS /////////////////////////////////////////////////////////////////////


