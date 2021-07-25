#ifndef LSS_H
#define LSS_H

#ifdef DEBUG
#undef DEBUG
#define DEBUG true
#else
#define DEBUG false
#endif

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
struct LSSNode *newUnionSet(struct LSSNode *setA, struct LSSNode *setB);
// Crea un conjunto formado por la intersección de dos conjuntos (sin repetidos)
struct LSSNode *newIntersectionSet(struct LSSNode *setA, struct LSSNode *setB);
// Retorna el conjunto partes (conjunto potencia) de un conjunto (sin repetidos)
struct LSSNode *newPowerSet(struct LSSNode *set);
//
bool            addStringToSet(struct LSSNode **set, char *elementAsString);
// Indica si dos conjuntos son iguales
bool            areSetsEqual(struct LSSNode *setA, struct LSSNode *setB);
// Indica si un elemento pertenece a un conjunto
bool            isElementOfSet(struct LSSNode *set);
// Retorna la cantidad de elementos de un conjunto
int             getSetCardinal(struct LSSNode *set);

// LISTAS (NODOS LST)

// Crea una lista vacia
struct LSSNode *newEmptyList(void);
// Inserta un elemento al final de una lista
bool            appendStringToList(struct LSSNode **list, char *elementAsString);
//
bool            isElementOfList(struct LSSNode *list);
//
int             getListSize(struct LSSNode *list);

// GENERAL

// Crear nodos y relaciones entre ellos según una cadena
struct LSSNode *newLSFromString(char *listOrSetAsString);
// Mostrar en consola toda la estuctura indicada
void            printLS(struct LSSNode *listOrSet);
// Elimina una estructura completamente, liberando la memoria asignada
void            freeLSS(struct LSSNode **listOrSetOrString);
// 
bool            hasDataLS(struct LSSNode *listOrSet);
// Devuelve un elemento según su posición. El elemento puede ser modificado.
struct LSSNode *getDataByPos(struct LSSNode **listOrSet, int dataPosition);
// 
struct LSSNode *getElementByPos(struct LSSNode **listOrSet, int elementPosition);
// Retorna un elemento como cadena
char           *getStringFromNode(struct LSSNode *listOrSet);
//
enum NODE_TYPE  getNodeType(struct LSSNode *listOrSetOrString);
//
int             compareLSS(struct LSSNode *listOrSetOrString1, struct LSSNode *listOrSetOrString2);

#endif
