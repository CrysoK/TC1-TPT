#ifndef LSS_H
#define LSS_H

// Librería externa "Simple Dynamic Strings"
#include "sds/sds.h"
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

// GENERAL

// Crear nodos y relaciones entre ellos según una cadena
struct LSSNode *newLSFromString(char *listOrSetAsString);
// Mostrar en consola toda la estuctura indicada
void            printLS(struct LSSNode *listOrSet);
// Elimina una estructura completamente, liberando la memoria asignada
void            freeLSS(struct LSSNode **listOrSetOrString);
// 
bool            isEmptyLS(struct LSSNode *set);

// CONJUNTOS

// Crea un conjunto vacío
struct LSSNode *newEmptySet();
// Crea un conjunto compuesto por la unión de dos conjuntos (sin repetidos)
struct LSSNode *newUnionSet(struct LSSNode *setA, struct LSSNode *setB);
// Crea un conjunto formado por la intersección de dos conjuntos (sin repetidos)
struct LSSNode *newIntersectionSet(struct LSSNode *setA, struct LSSNode *setB);
// Retorna el conjunto partes (conjunto potencia) de un conjunto (sin repetidos)
struct LSSNode *newPowerSet(struct LSSNode *set);
// Indica si dos conjuntos son iguales
bool            areSetsEqual(struct LSSNode *setA, struct LSSNode *setB);
// Indica si un elemento pertenece a un conjunto
bool            isElementOfSet(struct LSSNode *set);
// Retorna la cantidad de elementos de un conjunto
int             getSetCardinal(struct LSSNode *set);

// LISTAS

// Crea una lista vacia
struct LSSNode *newEmptyList();
// Inserta un elemento al final de una lista
void            appendStringToList(struct LSSNode **list, char *elementAsString);
//
bool            isElementOfList(struct LSSNode *list);
//
int             getListSize(struct LSSNode *list);

// CADENAS

// Retorna un elemento como cadena
char           *getStringFromNode(struct LSSNode *listOrSet);

#endif
