#include "LSS.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "sds/sds.h"
#include "natsort/strnatcmp.h"

// #s marca el inicio de una sección y !#s el final (VSCode CommentAnchors)

////////////////////////////////////////////////////////////////////////////////
// #S DECLARACIONES PRIVADAS ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// #S VARIABLES-CONSTANTES /////////////////////////////////////////////////////

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

// !#S

// #S PROTOTIPOS ///////////////////////////////////////////////////////////////

// #S GENERAL

// Mensajes de error
void            errorMessages(enum ERR code, const char *func, char *file, int line);
//
struct LSSNode *newLSFromStringRecursive(char * listOrSetAsString);
// 
struct LSSNode *newLSNode(enum NODE_TYPE typeToCreate);
//
struct LSSNode *cloneLSSNode(struct LSSNode *listOrSetorString);
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
//
int             compareStrings(char *string1, char *string2);
//
int             compareNodes(struct LSSNode *node1, struct LSSNode *node2);

// !#S

// #S NODOS SET

//
int             addNodeInOrder(struct LSSNode **firstElement, struct LSSNode *newElement);
//
int             getDataPriority(struct LSSNode * dataNode);

// !#S

// #S NODOS LST


// !#S

// #S NODOS STR

//
struct LSSNode *newStringNode(char * string);
// Extrae la cadena encerrada en llaves o corchetes
char          **splitString(char * string, int * sizeOfReturnedArray);
// Quita espacios alrededor de un elemento
void            deleteStringSpaces(char * string);
// Verifica que la entrada sea un conjunto o una lista
bool            isStringAListOrSet(char * string, bool isStringAnError);

// !#S:!#S:!#S

////////////////////////////////////////////////////////////////////////////////
// #S DEFINICIONES PÚBLICAS ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// #S GENERAL //////////////////////////////////////////////////////////////////

// 
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
    (*sl)->str = NULL;
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
  while(currentPos < pos && currentElement != NULL) {
    currentElement = currentElement->next;
    currentPos++;
  }
  // Si existe el elemento, retornarlo
  if(currentElement) return currentElement;
  else return NULL;
}

struct LSSNode *getDataByPos(struct LSSNode **ls, int pos) {
  return getElementByPos(ls, pos)->data;
}

char *getStringFromNode(struct LSSNode *ls) {
  if(!ls) return NULL;
  else if(ls->type == STR) return ls->str;
  else if(ls->data != NULL) return getStringFromNode(ls->data);
  else return NULL;
}

enum NODE_TYPE  getNodeType(struct LSSNode *lss) {
  if(lss == NULL) return -1;
  else return lss->type;
}

// node1 > node2 return 1
// node1 < node2 return -1
// node1 = node2 return 0
int compareNodes(struct LSSNode *node1, struct LSSNode *node2) {
  int returnValue = 0;
  int type1 = getDataPriority((*node1).data);
  int type2 = getDataPriority((*node2).data);
  // Comparación de prioridades
  if(type1 > type2) returnValue = 1;
  else if(type1 < type2) returnValue = -1;
  // Comparación de cadenas
  else if(type1 == 1) {
    if(compareStrings(node1->data->str, node2->data->str) > 0) returnValue = 1;
    else if(compareStrings(node1->data->str, node2->data->str) < 0) returnValue = -1;
    else returnValue = 0;
    // Comparación de conjuntos y listas
  } else {
    int size1 = getSizeOfLS(node1->data);
    int size2 = getSizeOfLS(node2->data);
    if(size1 > size2) returnValue = 1;
    else if(size1 < size2) returnValue = -1;
    else {
      int resComp = compareLSS(node1->data, node2->data);
      if(resComp > 0) returnValue = 1;
      else if(resComp < 0) returnValue = -1;
      else returnValue = 0;
    }
  }
  if(DEBUG) printf("<?> El resultado de \"%s\" es %d\n", __func__, returnValue);
  return returnValue;
}

int compareLSS(struct LSSNode *node1, struct LSSNode *node2) {
  int returnValue = compareNodes(node1, node2);
  if(returnValue == 0 && node1->next != NULL && node2->next != NULL)
    returnValue = compareLSS(node1->next, node2->next);
  if(DEBUG) printf("<?> El resultado de \"%s\" es %d\n", __func__, returnValue);
  return returnValue;
}

/*// LSS1 > LSS2 return 1
// LSS1 < LSS2 return -1
// LSS1 = LSS2 return 0
int compareLSS(struct LSSNode *node1, struct LSSNode *node2) {
  int returnValue = 0;
  int type1 = getDataPriority((*node1).data);
  int type2 = getDataPriority((*node2).data);
  // Comparación de prioridades
  if(type1 > type2) returnValue = 1;
  else if(type1 < type2) returnValue = -1;
  // Comparación de cadenas
  else if(type1 == 1) {
    if(compareStrings(node1->data->str, node2->data->str) > 0) returnValue = 1;
    else if(compareStrings(node1->data->str, node2->data->str) < 0) returnValue = -1;
    else if(node1->next != NULL && node2->next != NULL) return compareLSS(node1->next, node2->next);
    else if(node1->next != NULL && node2->next == NULL) returnValue = 1;
    else if(node1->next == NULL && node2->next != NULL) returnValue = -1;
    // Comparación de conjuntos y listas
  } else {
    int size1 = getSizeOfLS(node1->data);
    int size2 = getSizeOfLS(node2->data);
    if(size1 > size2) returnValue = 1;
    else if(size1 < size2) returnValue = -1;
    else {
      int resComp = compareLSS(node1->data, node2->data);
      if(resComp > 0) returnValue = 1;
      else if(resComp < 0) returnValue = -1;
      else if(node1->next != NULL) returnValue = compareLSS(node1->next, node2->next);
    }
  }
  if(DEBUG) printf("<?> El resultado de \"%s\" es %d\n", __func__, returnValue);
  return returnValue;
}*/

// !#S

// #S CONJUNTOS ////////////////////////////////////////////////////////////////

int getSetCardinal(struct LSSNode *node) {
  if(node->type != SET) return -1;
  else return getSizeOfLS(node);
}

struct LSSNode *newEmptySet(void) {
  struct LSSNode *new = newLSNode(SET);
  return new;
}

struct LSSNode *newIntersectionSet(struct LSSNode *setA, struct LSSNode *setB) {
  if(setA == NULL || setB == NULL) return NULL;
  struct LSSNode *intersection = newLSNode(SET);
  if(intersection == NULL) return NULL;
  int sizeA = getSizeOfLS(setA);
  int sizeB = getSizeOfLS(setB);
  int i = 1;
  int j = 1;

  while(i <= sizeA && j <= sizeB) {
    // A[i] = B[j]
    if(compareLSS(getElementByPos(&setA, i), getElementByPos(&setB, j)) == 0) {
      if(hasDataLS(intersection)) {
        struct LSSNode *setNode = newLSNode(SET);
        if(setNode == NULL) {
          errorMessages(MALLOC, __func__, __FILE__, __LINE__);
          return false;
        }
        linkNodeAsData(setNode, cloneLSSNode(getDataByPos(&setA, i)));
        addNodeInOrder(&intersection, setNode);
      } else linkNodeAsData(intersection, cloneLSSNode(getDataByPos(&setA, i)));
      i++;
      j++;
      // A[i] < B[j]
    } else if(compareLSS(getElementByPos(&setA, i), getElementByPos(&setB, j)) < 0) {
      i++;
      // A[i] > B[j]
    } else if(compareLSS(getElementByPos(&setA, i), getElementByPos(&setB, j)) > 0) {
      j++;
    }
  }
  return intersection;
}

bool addStringToSet(struct LSSNode **setRoot, char *string) {
  if((*setRoot) == NULL) {
    printf("<x> El primer argumento de \"%s\" es NULL <%s:%d>\n", __func__, __FILE__, __LINE__);
    return false;
  }
  if((*setRoot)->type != SET) {
    printf("<x> El primer argumento de \"%s\" no es un conjunto <%s:%d>\n", __func__, __FILE__, __LINE__);
    return false;
  }
  if(string == NULL) {
    printf("<x> El segundo argumento de \"%s\" es NULL <%s:%d>\n", __func__, __FILE__, __LINE__);
    return false;
  }
  struct LSSNode * data;
  if(isStringAListOrSet(string, false)) {
    if(DEBUG) printf("<?> Insertando un conjunto o lista al conjunto\n");
    data = newLSFromString(string);
  } else {
    if(DEBUG) printf("<?> Insertando una cadena al conjunto\n");
    data = newStringNode(string);
  }
  if(data == NULL) {
    errorMessages(MALLOC, __func__, __FILE__, __LINE__);
    return false;
  }
  if(hasDataLS(*setRoot)) {
    struct LSSNode *setNode = newLSNode(SET);
    if(setNode == NULL) {
      errorMessages(MALLOC, __func__, __FILE__, __LINE__);
      return false;
    }
    linkNodeAsData(setNode, data);
    addNodeInOrder(setRoot, setNode);
  } else linkNodeAsData(*setRoot, data);
  return true;
}

// !#S

// #S LISTAS ///////////////////////////////////////////////////////////////////

struct LSSNode *newEmptyList(void) {
  struct LSSNode * new = newLSNode(LST);
  return new;
}

bool appendStringToList(struct LSSNode ** list, char * string) {
  if((*list)->type != LST) {
    printf("<x> El primer argumento de \"%s\" no es una lista <%s:%d>\n", __func__, __FILE__, __LINE__);
    return false;
  }
  struct LSSNode * data;
  if(isStringAListOrSet(string, false)) {
    if(DEBUG) printf("<?> Insertando un conjunto o lista a la lista\n");
    data = newLSFromString(string);
  } else {
    if(DEBUG) printf("<?> Insertando una cadena a la lista\n");
    data = newStringNode(string);
  }
  if(data == NULL) {
    errorMessages(MALLOC, __func__, __FILE__, __LINE__);
    return false;
  }
  if(hasDataLS(*list)) {
    struct LSSNode *lstNode = newLSNode(LST);
    if(lstNode == NULL) {
      errorMessages(MALLOC, __func__, __FILE__, __LINE__);
      return false;
    }
    linkNodeAsData(lstNode, data);
    linkNodeAsNext(getLastNode(*list), lstNode);
  } else linkNodeAsData(*list, data);
  return true;
}

int getListSize(struct LSSNode *list) {
  if(list->type != LST) return -1;
  return getSizeOfLS(list);
}

// !#S:!#S

////////////////////////////////////////////////////////////////////////////////
// #S DEFINICIONES PRIVADAS ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// #S GENERAL //////////////////////////////////////////////////////////////////

void errorMessages(enum ERR key, const char *func, char * file, int line) {
  switch(key) {
    case MALLOC:
      printf("<x> No se pudo reservar la memoria necesaria <%s><%s:%d>\n", func, file, line);
      break;
    case NOT_SET:
      printf("<x> Falta una o mas llaves <%s><%s:%d>\n", func, file, line);
      break;
    case NOT_LST:
      printf("<x> Falta uno o mas corchetes <%s><%s:%d>\n", func, file, line);
      break;
    case NOT_LS:
      printf("<x> La entrada no es un conjunto ({ }) ni una lista ([ ]) <%s><%s:%d>\n", func, file, line);
      break;
    default:
      printf("<x> Codigo de error desconocido <%s><%s:%d>\n", func, file, line);
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
  nuevo = malloc(sizeof(*nuevo));
  nuevo->type = type;
  nuevo->data = NULL;
  nuevo->next = NULL;
  return nuevo;
}

struct LSSNode *newStringNode(char *str) {
  struct LSSNode *nuevo;
  nuevo = malloc(sizeof(*nuevo));
  nuevo->type = STR;
  // Eliminar espacios alrededor del elemento
  deleteStringSpaces(str);
  nuevo->str = sdsnew(str);
  return nuevo;
}

struct LSSNode *cloneLSSNode(struct LSSNode *lss) {
  struct LSSNode *newNode = malloc(sizeof(*newNode));
  newNode->type = lss->type;
  if(newNode->type != STR) {
    newNode->data = cloneLSSNode(lss->data);
    newNode->next = cloneLSSNode(lss->next);
  } else newNode->str = sdsnew(lss->str);
  return newNode;
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
      if(isStringAnError) errorMessages(NOT_LS, __func__, __FILE__, __LINE__);
      return false; // Se omite el resto del análisis
      break;
  }
  // Control balance de {} y []
  for(size_t i = 1; i < len; i++) {
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
        if(isStringAnError) errorMessages(NOT_LS, __func__, __FILE__, __LINE__);
      }
      // Se impide cambiar el valor de isListOrSet nuevamente
      isFirstTime = false;
    }
  }
  // Si falta un corchete o una llave, se considera no válido
  if(setCount != 0) {
    errorMessages(NOT_SET, __func__, __FILE__, __LINE__);
    isListOrSet = false;
  }
  if(listCount != 0) {
    errorMessages(NOT_LST, __func__, __FILE__, __LINE__);
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
  array = malloc(sizeof(*array) * mem);
  if(array == NULL) return NULL; // ERROR en la asignación
  // Caso en el que la cadena no tiene caracteres
  if(lar == 0) { *cant = 0; return array; }

  // Mientras no sea el fin de la cadena
  while(str[i] != '\0') {
    // Asignación de memoria extra si es necesario
    if(mem < elem + 2) {
      mem *= 2;
      char **aux = realloc(array, sizeof(*array) * mem);
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
  for(int j = 0; j < elem; j++) sdsfree(array[j]); // Se liberan las char *
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

bool hasDataLS(struct LSSNode * sl) {
  if(sl->data != NULL) return true;
  else {
    if(DEBUG) printf("<?> Nodo LST/SET sin dato detectado\n");
    return false;
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
  // Control nodo válido
  if(node == NULL) return -1;
  
  if(node->type == STR) return -1;
  // Control conjunto vacío
  if(!hasDataLS(node)) return 0;

  int cnt = 0;
  while(node != NULL) {
    cnt++;
    node = node->next;
  }
  return cnt;
}

/* Comparación de cadenas "natural" o "numérica":
 * - Split each filename into alphabetical and numeric parts;
 *   i.e. the name text123moretext456 becomes the list {"text", "123", "moretext", "456"}
 * - For each part in the two split names, perform the following comparison:
 *   - If both parts are strictly numeric, compare them as numbers
 *     - If the numbers are the same, compare them as strings
 *     - If the strings are the same, move on to the next part
 *   - If both parts are strictly letters, compare them as strings
 *     - If they're the same, move on to the next part
 * - If you run out of parts, the name with the fewest parts comes first
**/

/* string1 > string2 return 1
 * string1 < string2 return -1
 * string1 = string2 return 0
**/
int compareStrings(char *string1, char *string2) {
  // strnatcmp no tiene en cuenta números negativos por lo que se realiza una
  // comparación simple si alguna de las cadenas es un número negativo
  if(isStringANumber(string1) && isStringANumber(string2)) {
    double num1 = atof(string1);
    double num2 = atof(string2);
    if(num1 < 0 || num2 < 0){
      if(num1 > num2) return 1;
      else if(num1 < num2) return -1;
      else return 0;
    }
  }
  return strnatcmp(string1, string2);
}

// !#S

// #S CONJUNTOS ////////////////////////////////////////////////////////////////

int getDataPriority(struct LSSNode * data) {
  if(data == NULL) return 0; // El nodo existe
  int dataType;
  if(data->type == STR) dataType = 1;
  else dataType = (data->type == SET) ? 2 : 3;
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
  int cmpNodes = compareLSS(newNode, currentNode);
  // Búsqueda de la posición
  while(cmpNodes > 0 && currentNode->next != NULL) {
    if(DEBUG) printf("<?> \"%s\" es mayor que \"%s\"\n", newNode->data->str, currentNode->data->str);
    previousNode = currentNode;
    currentNode = currentNode->next;
    currentPos++;
    cmpNodes = compareLSS(newNode, currentNode);
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

// !#S

// #S LISTAS ///////////////////////////////////////////////////////////////////



// !#S

// #S CADENAS //////////////////////////////////////////////////////////////////


// !#S:!#S
