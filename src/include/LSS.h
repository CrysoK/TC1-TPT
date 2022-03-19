#ifndef LSS_H
#define LSS_H

#define DEBUG 0

// Librería estándar que incluye el tipo de dato booleano
#include <stdbool.h>

// ESTRUCTURA //////////////////////////////////////////////////////////////////

// Tipo de nodo: LST, SET, STR
enum NODE_TYPE { LST, SET, STR };

// Registro "nodo". Unidad de la estructura
struct LSSNode /* dataType */ {
  enum NODE_TYPE type; // nodeType
  union {
    char *str; // dataStr
    struct {
      struct LSSNode *data; // data
      struct LSSNode *next; // next
    };
  };
};

// OPERACIONES /////////////////////////////////////////////////////////////////

// CONJUNTOS (NODOS SET)

// Crea un conjunto vacío
struct LSSNode *newEmptySet(void);
// Crea un conjunto compuesto por la unión de dos conjuntos (sin repetidos)
// struct LSSNode *newUnionSet(struct LSSNode *setA, struct LSSNode *setB);
// Crea un conjunto formado por la intersección de dos conjuntos (sin repetidos)
struct LSSNode *newIntersectionSet(struct LSSNode *setA, struct LSSNode *setB);
// Retorna el conjunto partes (conjunto potencia) de un conjunto (sin repetidos)
// struct LSSNode *newPowerSet(struct LSSNode *set);
// Añade un elemento a un conjunto de forma ordenada y evitando repetidos.
// Devuelve -1 en caso de error y 0 si el elemento ya formaba parte del conjunto.
// El elemento NO se elimina si se encuentra repetido.
int             addElementToSet(struct LSSNode **set, struct LSSNode *elementAsNode);
// Retorna la cantidad de elementos de un conjunto
int             getSetCardinal(struct LSSNode *set);

// LISTAS (NODOS LST)

// Crea una lista vacía
struct LSSNode *newEmptyList(void);
// Inserta un elemento al final de una lista
bool            appendElementToList(struct LSSNode **list, struct LSSNode *elementAsNode);
// Devuelve la cantidad de elementos de una lista
int             getListSize(struct LSSNode *list);

// GENERAL

// Crear nodos y relaciones entre ellos según una cadena
struct LSSNode *newLssFromString(char *listOrSetAsString);
// Duplica una estructura y todos sus elementos en una nueva del tipo indicado, la original no se elimina.
struct LSSNode *cloneLss(struct LSSNode *listOrSetorString, enum NODE_TYPE newType);
// Mostrar en consola toda la estructura indicada
void            printLss(struct LSSNode *listOrSet);
//
void            generateDotFile(struct LSSNode *listOrSet, char *fileName);
// Elimina una estructura completamente, liberando la memoria asignada
void            freeLss(struct LSSNode **listOrSetOrString);
// Indica si una lista o un conjunto tiene al menos un elemento
bool            hasDataLs(struct LSSNode *listOrSet);
// Comparación entre estructuras de cualquier tipo (LST, SET, STR)
int             compareLss(struct LSSNode *listOrSetOrString, struct LSSNode *istOrSetOrString);
// Devuelve un elemento según su posición
struct LSSNode *getElementByPos(struct LSSNode **listOrSet, int elementPos);
// Devuelve la posición de un elemento en listas y conjuntos, 0 si no se encontró y -1 si hubo algún error
int             getPosByElement(struct LSSNode *listOrSet, struct LSSNode *elementToSearch);
// Retorna un elemento como cadena
char           *getElementString(struct LSSNode *listOrSet);
// Devuelve el tipo de un elemento (LST, SET, STR)
enum NODE_TYPE  getElementType(struct LSSNode *listOrSetOrString);

#endif
