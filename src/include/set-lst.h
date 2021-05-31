#ifndef SETLST_H
#define SETLST_H

// Librería externa "Simple Dynamic Strings"
#include "sds/sds.h"
// Librería estándar que incluye el tipo de dato booleano
#include <stdbool.h>

// ESTRUCTURA //////////////////////////////////////////////////////////////////

// Tipo de nodo: SET, LST, STR
enum TIPO_NODO { SET, LST, STR };

// Registro "nodo". Unidad de la estructura
struct sl_nodo /* dataType */ {
  enum TIPO_NODO tipo; // nodeType
  union {
    char *str; // dataStr
    struct {
      struct sl_nodo *dato; // data
      struct sl_nodo *sig; // next
    };
  };
};

// OPERACIONES /////////////////////////////////////////////////////////////////

// GENERAL

// Crear nodos y relaciones entre ellos según una cadena
struct sl_nodo *sl_nuevo(char * cadena);
// Mostrar en consola toda la estuctura indicada
void            sl_mostrar(struct sl_nodo * sl);
// Elimina una estructura completamente, liberando la memoria asignada
void sl_free(struct sl_nodo ** ls);

// CONJUNTOS

// Crea un conjunto vacío
struct sl_nodo *set_vacio();
// Crea un conjunto compuesto por la unión de dos conjuntos (sin repetidos)
struct sl_nodo *set_union(struct sl_nodo* set_1, struct sl_nodo* set_2);
// Crea un conjunto formado por la intersección de dos conjuntos (sin repetidos)
struct sl_nodo *set_inter(struct sl_nodo* set_1, struct sl_nodo* set_2);
// Retorna el conjunto partes de un conjunto (sin repetidos)
struct sl_nodo *set_partes(struct sl_nodo* set);
// Indica si dos conjuntos son iguales
bool            set_igual(struct sl_nodo* set_1, struct sl_nodo* set_2);
// Indica si un elemento pertenece a un conjunto
bool            set_perten(struct sl_nodo* set);
// Retorna la cantidad de elementos de un conjunto
int             set_cardinal(struct sl_nodo* set);

// LISTAS

// Crea una lista vacia
struct sl_nodo *lst_vacia();
// Inserta una lista al final de otra
void            lst_final(struct sl_nodo ** lista, struct sl_nodo * nuevo);

// CADENAS

// Retorna un elemento en formato cadena (sds)
char           *str_return(struct sl_nodo * sl);

#endif
