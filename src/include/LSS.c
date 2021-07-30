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
struct LSSNode *newLsFromStringRecursive(char * listOrSetAsString);
//
char           *stringFromLss(char *string, struct LSSNode * nodo, bool primero);
// 
struct LSSNode *newLSNode(enum NODE_TYPE typeToCreate);
//
void            printLsRecursive(struct LSSNode *listOrSet, bool isFirstElement, int *count);
//
void            genDotNodes(FILE *file, struct LSSNode *actual, int *id, struct LSSNode *previo, int idPrev, int dataOrNext);
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
// Verifica que la entrada sea un conjunto o una lista
bool            isStringAListOrSet(char * string);

// !#S:!#S:!#S

////////////////////////////////////////////////////////////////////////////////
// #S DEFINICIONES PÚBLICAS ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// #S GENERAL //////////////////////////////////////////////////////////////////

// 
struct LSSNode *newLssFromString(char *input) {
  // Tipo a crear
  if(isStringAListOrSet(input)) {
    // Retorna la creación recursiva
    return newLsFromStringRecursive(input);;
  } else return newStringNode(input);
}

void printLss(struct LSSNode * node) {
  // Comprobación de que exista al menos un nodo
  if(node == NULL) {
    printf("No hay datos");
    return;
  }
  int count = 0;
  printLsRecursive(node, true, &count);
}

void genDotNodes(FILE *file, struct LSSNode *actual, int *id, struct LSSNode *prev, int idPrev, int dataOrNext) {
  int aux = *id;
  // Caso del conjunto vacío o lista vacía
  if(actual == NULL) {
    fprintf(file, "\tnodo%d[label=\"NULL\" shape=none]\n", *id);
    if(prev != NULL) {
      if(dataOrNext == 0) {
        fprintf(file, "\tnodo%d:data->nodo%d\n", idPrev, *id);
      } else fprintf(file, "\tnodo%d:next->nodo%d\n", idPrev, *id);
    }
    *id = *id + 1;
    return;
  }
  enum NODE_TYPE type = actual->type;
  if(type == STR) {
    // Se muestra la cadena
    if(actual->str[0] == '{') {
      char *temp = sdsnewlen(actual->str + 1, strlen(actual->str) - 2);
      fprintf(file, "\tnodo%d[label=\"<type>STR|<str>\\\"\\{%s\\}\\\"\"]\n", *id, temp);
      sdsfree(temp);
    } else fprintf(file, "\tnodo%d[label=\"<type>STR|<str>\\\"%s\\\"\"]\n", *id, actual->str);
    if(prev != NULL) {
      if(dataOrNext == 0) {
        fprintf(file, "\tnodo%d:data->nodo%d:type\n", idPrev, *id);
      } else fprintf(file, "\tnodo%d:next->nodo%d:type\n", idPrev, *id);
    }
    *id = *id + 1;
    return;
  } else if(type == SET) {
    fprintf(file, "\tnodo%d[label=\"<type>SET|<data>data|<next>next\" color=blue]\n", *id);
    if(prev != NULL) {
      if(dataOrNext == 0) {
        fprintf(file, "\tnodo%d:data->nodo%d:type\n", idPrev, *id);
      } else fprintf(file, "\tnodo%d:next->nodo%d:type\n", idPrev, *id);
    }
    *id = *id + 1;
  } else {
    fprintf(file, "\tnodo%d[label=\"<type>LST|<data>data|<next>next\" color=green]\n", *id);
    if(prev != NULL) {
      if(dataOrNext == 0) {
        fprintf(file, "\tnodo%d:data->nodo%d:type\n", idPrev, *id);
      } else fprintf(file, "\tnodo%d:next->nodo%d:type\n", idPrev, *id);
    }
    *id = *id + 1;
  }
  // Se muestra el dato
  genDotNodes(file, actual->data, id, actual, aux, 0);
  genDotNodes(file, actual->next, id, actual, aux, 1);
}

void generateDotFile(struct LSSNode *lss) {

  // open file in write mode
  FILE *file = fopen("LSS.dot", "w");
  if(file == NULL) {
    printf("<x> No se pudo abrir el archivo\n");
    return;
  }

  fprintf(file, "digraph AF {\n\tgraph [center=true splines=true]\n\tnode[shape=record]\n");

  int id = 0;
  genDotNodes(file, lss, &id, NULL, 0, 0);

  fprintf(file, "}");

  fclose(file);
}

void freeLss(struct LSSNode ** sl) {
  struct LSSNode *aux = *sl;
  // Exit si el nodo no existe
  if(aux == NULL) return;
  if(aux->type != STR) {
    // Se libera el subconjunto o sublista
    freeLss(&(aux->data));
    // Se libera el siguiente elemento
    freeLss(&(aux->next));
  } else {
    // Se libera la cadena
    sdsfree(aux->str);
    aux->str = NULL;
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
  if(currentElement) return currentElement->data;
  else return NULL;
}

int getPosByElement(struct LSSNode *ls, struct LSSNode *element) {
  if(!ls) return -1;
  if(ls->type == STR) return -1;
  if(DEBUG) { printf("Buscando \"");printLss(element);printf("\"\n"); }
  int pos = 1;
  while(ls != NULL) {
    int comp = compareLss(ls->data, element);

    // En el caso de conjuntos, si el elemento es menor, no es necesario seguir buscando
    // (Todavía no se puede asegurar que un conjunto esté siempre ordenado)
    // if(ls->type == SET && comp > 0) return 0;

    // Elemento encontrado
    if(comp == 0) return pos;

    // Siguiente
    ls = ls->next;
    pos++;
  }
  // No hay más para comparar, elemento no encontrado
  return 0;
}

char *getElementString(struct LSSNode *ls) {
  if(!ls) return NULL;
  else if(ls->type == STR) return ls->str;
  else if(ls->data != NULL) return getElementString(ls->data);
  else return NULL;
}

enum NODE_TYPE  getElementType(struct LSSNode *lss) {
  if(lss == NULL) return -1;
  else return lss->type;
}

// node1 > node2 return 1
// node1 < node2 return -1
// node1 = node2 return 0
int compareLss(struct LSSNode *node1, struct LSSNode *node2) {
  int returnValue = 0;
  int type1 = getDataPriority(node1);
  int type2 = getDataPriority(node2);
  // Comparación de prioridades
  if(type1 > type2) returnValue = 1;
  else if(type1 < type2) returnValue = -1;
  // Ambos son NULL
  else if(type1 == 0) returnValue = 0;
  // Comparación de cadenas
  else if(type1 == 1) {
    if(compareStrings(node1->str, node2->str) > 0) returnValue = 1;
    else if(compareStrings(node1->str, node2->str) < 0) returnValue = -1;
    else returnValue = 0;
    // Comparación de conjuntos y listas
  } else {
    int size1 = getSizeOfLS(node1);
    int size2 = getSizeOfLS(node2);
    if(size1 > size2) returnValue = 1;
    else if(size1 < size2) returnValue = -1;
    else {
      int resComp = compareLss(node1->data, node2->data);
      if(resComp > 0) returnValue = 1;
      else if(resComp < 0) returnValue = -1;
      else returnValue = compareLss(node1->next, node2->next);
    }
  }
  //if(DEBUG) printf("<?> El resultado de \"%s\" es %d\n", __func__, returnValue);
  return returnValue;
}

struct LSSNode *cloneLss(struct LSSNode *lss, enum NODE_TYPE type) {
  if(lss == NULL) return NULL;

  if(lss->type == STR && type != STR) {
    printf("<x> No se puede convertir de forma directa un nodo STR en uno LST o SET. Usa \"newLssFromString\".\n");
    return NULL;
  }

  struct LSSNode *newNode;
  newNode = newLSNode(type);
  if(newNode == NULL) {
    errorMessages(MALLOC, __func__, __FILE__, __LINE__);
    return NULL;
  }
  if(type != STR) {
    if(lss->data != NULL) {
      newNode->data = cloneLss(lss->data, lss->data->type);
    } else newNode->data = NULL;
    newNode->next = cloneLss(lss->next, type);
  } else {
    newNode->str = stringFromLss(sdsempty(), lss, true);
  }
  return newNode;
}

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

  while(setA != NULL && setB != NULL) {
    int comp = compareLss(setA->data, setB->data);
    // A[i] = B[j]
    if(comp == 0) {
      if(hasDataLs(intersection)) {
        struct LSSNode *setNode = newLSNode(SET);
        if(setNode == NULL) {
          errorMessages(MALLOC, __func__, __FILE__, __LINE__);
          return NULL;
        }
        setNode->data = cloneLss(setA->data, setA->data->type);
        int res = addNodeInOrder(&intersection, setNode);
        if(res <= 0) {
          freeLss(&setNode);
          return NULL;
        }
      } else intersection->data = cloneLss(setA->data, setA->data->type);
      setA = setA->next;
      setB = setB->next;
      // A[i] < B[j]
    } else if(comp < 0) {
      setA = setA->next;
      // A[i] > B[j]
    } else if(comp > 0) {
      setB = setB->next;
    }
  }
  return intersection;
}

int addElementToSet(struct LSSNode **setRoot, struct LSSNode *node) {
  if((*setRoot) == NULL) {
    printf("<x> El primer argumento de \"%s\" es NULL <%s:%d>\n", __func__, __FILE__, __LINE__);
    return -1;
  }
  if((*setRoot)->type != SET) {
    printf("<x> El primer argumento de \"%s\" no es un conjunto <%s:%d>\n", __func__, __FILE__, __LINE__);
    return -1;
  }
  if(node == NULL) {
    printf("<x> El segundo argumento de \"%s\" es NULL <%s:%d>\n", __func__, __FILE__, __LINE__);
    return -1;
  }
  if(hasDataLs(*setRoot)) {
    struct LSSNode *setNode = newLSNode(SET);
    if(setNode == NULL) {
      errorMessages(MALLOC, __func__, __FILE__, __LINE__);
      return -1;
    }
    setNode->data = node;
    int res = addNodeInOrder(setRoot, setNode);
    if(res <= 0) free(setNode);
    return res;
  } else {
    (*setRoot)->data = node;
    return 1;
  }
}

// !#S

// #S LISTAS ///////////////////////////////////////////////////////////////////

struct LSSNode *newEmptyList(void) {
  struct LSSNode * new = newLSNode(LST);
  return new;
}

bool appendElementToList(struct LSSNode **list, struct LSSNode *node) {
  if((*list) == NULL) {
    printf("<x> El primer argumento de \"%s\" es NULL <%s:%d>\n", __func__, __FILE__, __LINE__);
    return false;
  }
  if((*list)->type != LST) {
    printf("<x> El primer argumento de \"%s\" no es una lista <%s:%d>\n", __func__, __FILE__, __LINE__);
    return false;
  }
  if(node == NULL) {
    printf("<x> El segundo argumento de \"%s\" es NULL <%s:%d>\n", __func__, __FILE__, __LINE__);
    return false;
  }
  if(hasDataLs(*list)) {
    struct LSSNode *lstNode = newLSNode(LST);
    if(lstNode == NULL) {
      errorMessages(MALLOC, __func__, __FILE__, __LINE__);
      return false;
    }
    linkNodeAsData(lstNode, node);
    linkNodeAsNext(getLastNode(*list), lstNode);
  } else linkNodeAsData(*list, node);
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

struct LSSNode *newLsFromStringRecursive(char *string) {
  // Crear raiz
  struct LSSNode *rootNode = NULL;
  // De qué tipo es string
  enum NODE_TYPE stringType = getStringType(string);

  char **elementsArray;
  int elementsCount = 0;
  // Crear un array con los elementos de string
  elementsArray = splitString(string, &elementsCount);

  if(elementsCount > 0) { // Hay elementos
    for(int index = 0; index < elementsCount; index++) { // Por cada elemento
      struct LSSNode *newNode = newLSNode(stringType); // Nodo para el nuevo elemento, todos del mismo tipo
      if(getStringType(elementsArray[index]) != STR) { // El elemento es un subconjunto/sublista
        // Crear el subset/sublst y unirlo como elemento del actual (como dato)
        linkNodeAsData(newNode, newLsFromStringRecursive(elementsArray[index]));
      } else { // El elemento es STR
        // Si la cadena no es vacía, crear el nodo STR y unirlo como elemento (como dato)
        elementsArray[index] = sdstrim(elementsArray[index], " ");
        if(sdscmp(elementsArray[index], "") != 0) linkNodeAsData(newNode, newStringNode(elementsArray[index]));
      }
      if(rootNode == NULL) rootNode = newNode; // Si raiz aún no apunta a un nodo, inicializar
      else if(stringType != SET) {
        // Enlazar newNode como el ultimo elemento del arbol
        linkNodeAsNext(getLastNode(rootNode), newNode);
      } else {
        // Insertar el nuevo nodo de forma ordenada solo cuando es elemento de un conjunto
        int res = addNodeInOrder(&rootNode, newNode);
        if(res <= 0) {
          freeLss(&newNode);
        }
      }
    } // <- Fin del ciclo
  }
  sdsfreesplitres(elementsArray, elementsCount); // Libera la memoria utilizada por el array
  return rootNode;
}

/* sdsValida sdscatprintf(sdsNoValida, ...);

sdsValida stringFromLss(sdsNoValida, nodo, primero){
  return sdsValida
}


*/

char *stringFromLss(char *string, struct LSSNode * nodo, bool primero) {
  // Caso del conjunto vacío o lista vacía
  if(nodo == NULL) {

    return sdscat(string, " ");
  }
  if(nodo->type == STR) {
    // Se muestra la cadena 

    return sdscat(string, nodo->str);;
  }
  // Se abre el conjunto o lista
  if(primero) {
    if(nodo->type == SET) string = sdscat(string, "{");
    else string = sdscat(string, "[");
  }
  // Se muestra el dato
  string = stringFromLss(string, nodo->data, true);
  if(nodo->next == NULL) {
    // Se cierra el conjunto o lista
    if(nodo->type == SET) return sdscat(string, "}");
    else return sdscat(string, "]");
  }
  // Se muestra el siguiente elemento
  string = sdscat(string, ",");
  return stringFromLss(string, nodo->next, false);
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
  nuevo->str = sdstrim(sdsnew(str), " ");
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

bool isStringAListOrSet(char *str) {
  if(str == NULL) return false;

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
      if(DEBUG) printf("<?> La entrada no es un conjunto ({ }) ni una lista "
      "([ ]) <%s><%s:%d>\n", __func__, __FILE__, __LINE__);
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
        errorMessages(NOT_LS, __func__, __FILE__, __LINE__);
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
  if(str == NULL) return STR;
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

  if(isStringAListOrSet(str)) { // Se confirma que str sí es un conjunto o una lista
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
void printLsRecursive(struct LSSNode * nodo, bool primero, int *count) {
  // hardlimit por si el LSS tiene algún error
  if(*count >= 100) {
    printf(" <x> Limite alcanzado ");
    return;
  }
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
  *count = *count + 1;
  printLsRecursive(nodo->data, true, count);
  if(nodo->next == NULL) {
    // Se cierra el conjunto o lista
    type == SET ? printf("}") : printf("]");
    return;
  }
  // Se muestra el siguiente elemento
  printf(",");
  *count = *count + 1;
  printLsRecursive(nodo->next, false, count);
}

bool hasDataLs(struct LSSNode * sl) {
  if(sl == NULL) return false;
  if(sl->type == STR) return true;
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
  if(!hasDataLs(node)) return 0;

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
    if(num1 < 0 || num2 < 0) {
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
    return -1;
  }
  if((*firstNode)->type != SET) {
    printf("<x> El primer argumento de \"%s\" debe ser un conjunto <%s:%d>\n", __func__, __FILE__, __LINE__);
    return -1;
  }
  struct LSSNode *currentNode = *firstNode;
  struct LSSNode *previousNode = *firstNode;
  int currentPos = 1;
  int cmpNodes = compareLss(newNode->data, currentNode->data);
  // Búsqueda de la posición
  while(cmpNodes > 0 && currentNode->next != NULL) {
    if(DEBUG) printf("<?> \"%s\" es mayor que \"%s\"\n", newNode->data->str, currentNode->data->str);
    previousNode = currentNode;
    currentNode = currentNode->next;
    currentPos++;
    cmpNodes = compareLss(newNode->data, currentNode->data);
  }
  if(cmpNodes == 0) {
    if(DEBUG) {
      printf("<?> Elemento repetido \"");
      printLss(newNode->data);
      printf("\"\n");
    }
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
    printLss(newNode->data);
    printf("\" insertado en la posicion %d\n", currentPos);
  }
  return currentPos;
}

// !#S

// #S LISTAS ///////////////////////////////////////////////////////////////////



// !#S

// #S CADENAS //////////////////////////////////////////////////////////////////



// !#S:!#S
