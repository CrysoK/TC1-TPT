#ifndef LSS_H
#define LSS_H

// Librería estándar que incluye el tipo de dato booleano
#include <stdbool.h>

// ESTRUCTURA //////////////////////////////////////////////////////////////////

// Tipo de nodo: LST, SET, STR
enum LSS_NodeType { LST, SET, STR };

// Registro "nodo". Unidad de la estructura
struct LSS_Node /* dataType */ {
  enum LSS_NodeType type; // nodeType
  union {
    char *str; // dataStr
    struct {
      struct LSS_Node *data; // data
      struct LSS_Node *next; // next
    };
  };
};

// OPERACIONES /////////////////////////////////////////////////////////////////

// CONJUNTOS (NODOS SET)

// Crea un conjunto vacío
struct LSS_Node  *LSS_SET_new_empty(void);
// Crea un conjunto compuesto por la unión de dos conjuntos (sin repetidos)
// struct LSS_Node *newUnionSet(struct LSS_Node *set_a, struct LSS_Node *set_b);
// Crea un conjunto formado por la intersección de dos conjuntos (sin repetidos)
struct LSS_Node  *LSS_SET_new_intersection(
  struct LSS_Node  *set_a,
  struct LSS_Node  *set_b
);
// Retorna el conjunto partes (conjunto potencia) de un conjunto (sin repetidos)
// struct LSS_Node *newPowerSet(struct LSS_Node *set);

// Añade un elemento a un conjunto de forma ordenada y evitando repetidos.
// Devuelve -1 en caso de error y 0 si el elemento ya formaba parte del
// conjunto. El elemento NO se elimina si se encuentra repetido.
int               LSS_SET_add_element(
  struct LSS_Node **set,
  struct LSS_Node  *element_node
);
// Retorna la cantidad de elementos de un conjunto
int               LSS_SET_cardinal(
  struct LSS_Node  *set
);

// LISTAS (NODOS LST)

// Crea una lista vacía
struct LSS_Node  *LSS_LST_new_empty(void);
// Inserta un elemento al final de una lista
bool              LSS_LST_append_element(
  struct LSS_Node **list,
  struct LSS_Node  *element_node
);
// Devuelve la cantidad de elementos de una lista
int               LSS_LST_size(
  struct LSS_Node  *list
);

// GENERAL

// Crear nodos y relaciones entre ellos según una cadena
struct LSS_Node  *LSS_from_string(
  char             *list_or_set_as_string
);
// Duplica una estructura y todos sus elementos en una nueva del tipo indicado,
// la original no se elimina.
struct LSS_Node  *LSS_clone(
  struct LSS_Node  *list_set_string,
  enum LSS_NodeType new_type
);
// Mostrar en consola toda la estructura indicada
void              LSS_print(
  struct LSS_Node  *list_set
);
//
void              LSS_gen_dotfile(
  struct LSS_Node  *list_set,
  char             *filename);
// Elimina una estructura completamente, liberando la memoria asignada
void              LSS_free(
  struct LSS_Node **list_set_string
);
// Indica si una lista o un conjunto tiene al menos un elemento
bool              LSS_LST_SET_has_data(
  struct LSS_Node  *list_set
);
// Comparación entre estructuras de cualquier tipo (LST, SET, STR)
int               LSS_compare(
  struct LSS_Node  *list_set_string_a,
  struct LSS_Node  *list_set_string_b
);
// Devuelve un elemento según su posición
struct LSS_Node  *LSS_get_elem_by_pos(
  struct LSS_Node **list_set,
  int               element_pos
);
// Devuelve la posición de un elemento en listas y conjuntos, 0 si no se
// encontró y -1 si hubo algún error
int               LSS_get_pos_by_elem(
  struct LSS_Node  *list_set,
  struct LSS_Node  *elem_to_search
);
// Retorna un elemento como cadena
char             *LSS_get_elem_string(
  struct LSS_Node  *list_set
);
// Devuelve el tipo de un elemento (LST, SET, STR)
enum LSS_NodeType LSS_get_elem_type(
  struct LSS_Node  *list_set_string
);

#endif
