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
struct NodoSLS /* dataType */ {
  enum TIPO_NODO tipo; // nodeType
  union {
    char *str; // dataStr
    struct {
      struct NodoSLS *dato; // data
      struct NodoSLS *sig; // next
    };
  };
};

// OPERACIONES /////////////////////////////////////////////////////////////////

// GENERAL

// Crear nodos y relaciones entre ellos según una cadena
struct NodoSLS *sl_nuevo(char * cadena);
// Mostrar en consola toda la estuctura indicada
void            sl_mostrar(struct NodoSLS * set_lst);
// Elimina una estructura completamente, liberando la memoria asignada
void sl_free(struct NodoSLS ** set_lst);

// CONJUNTOS

// Crea un conjunto vacío
struct NodoSLS *set_vacio();
// Crea un conjunto compuesto por la unión de dos conjuntos (sin repetidos)
struct NodoSLS *set_union(struct NodoSLS* set_1, struct NodoSLS* set_2);
// Crea un conjunto formado por la intersección de dos conjuntos (sin repetidos)
struct NodoSLS *set_inter(struct NodoSLS* set_1, struct NodoSLS* set_2);
// Retorna el conjunto partes de un conjunto (sin repetidos)
struct NodoSLS *set_partes(struct NodoSLS* set);
// Indica si dos conjuntos son iguales
bool            set_igual(struct NodoSLS* set_1, struct NodoSLS* set_2);
// Indica si un elemento pertenece a un conjunto
bool            set_perten(struct NodoSLS* set);
// Retorna la cantidad de elementos de un conjunto
int             set_cardinal(struct NodoSLS* set);

// LISTAS

// Crea una lista vacia
struct NodoSLS *lst_vacia();
// Inserta un elemento al final de una lista
void            lst_ins_final(struct NodoSLS ** lista, char * nuevo);

// CADENAS

// Retorna un elemento como cadena
char           *str_return(struct NodoSLS * set_lst);

#endif
