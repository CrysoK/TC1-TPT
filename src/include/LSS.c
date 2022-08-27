#include "LSS.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "sds/sds.h"
#include "natsort/strnatcmp.h"
#include "stack.h"
#include "debug.h"

// #s marca el inicio de una sección y !#s el final (VSCode CommentAnchors)

////////////////////////////////////////////////////////////////////////////////
// #S DECLARACIONES PRIVADAS ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// #S VARIABLES-CONSTANTES /////////////////////////////////////////////////////

// Códigos de error
enum Err {
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
void              error_messages(
  enum Err          code,
  const char       *func,
  char             *file,
  int               line
);
//
struct LSS_Node  *new_ls_from_string_recursive(
  char             *list_or_set_as_string
);
//
char             *lss_to_string(
  char             *string,
  struct LSS_Node  *node,
  bool              first
);
// 
struct LSS_Node  *new_ls_node(
  enum LSS_NodeType type
);
//
void              print_recursive(
  struct LSS_Node  *list_set,
  bool              first,
  int              *count
);
//
void              gen_dot_nodes(
  FILE             *file,
  struct LSS_Node  *current,
  int              *id,
  struct LSS_Node  *prev,
  int               id_prev,
  int               data_or_next
);
//
void              link_node_data(
  struct LSS_Node  *node,
  struct LSS_Node  *data_node
);
//
void              link_node_next(
  struct LSS_Node  *node,
  struct LSS_Node  *next_node
);
// 
struct LSS_Node  *get_last_node(
  struct LSS_Node  *list_set
);
// Devuelve si la entrada es un conjunto, una lista o una cadena. La cadena no
// debe tener espacios al inicio ni al final.
enum LSS_NodeType get_string_type(
  char             *string
);
//
bool              is_string_element_of_ls(
  struct LSS_Node  *list_set,
  char             *element_as_string
);
//
bool              is_string_a_number(
  char             *string
);
//
int               get_ls_size(
  struct LSS_Node  *list_set
);
//
int               compare_strings(
  char             *string_a,
  char             *string_b
);

// !#S

// #S NODOS SET

//
int               add_node_in_order(
  struct LSS_Node **first_element,
  struct LSS_Node  *new_element
);
//
int               get_data_priority(
  struct LSS_Node  *data_node
);

// !#S

// #S NODOS LST


// !#S

// #S NODOS STR

//
struct LSS_Node  *new_str_node(
  char             *string);
// Extrae la cadena encerrada en llaves o corchetes
char            **split_string(
  char             *string,
  int              *returned_array_size
);
// Verifica que la cadena sea un lista/conjunto/cadena válido
bool              is_string_a_valid_lss(
  char             *string
);
// Verifica que la entrada sea un conjunto o una lista
bool              is_string_a_list_or_set(
  char             *string
);

// !#S:!#S:!#S

////////////////////////////////////////////////////////////////////////////////
// #S DEFINICIONES PÚBLICAS ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// #S GENERAL //////////////////////////////////////////////////////////////////

// 
struct LSS_Node *LSS_from_string(char *input) {
  sdstrim(input, " ");
  // Tipo a crear
  if(get_string_type(input) == STR) {
    return new_str_node(input);
  }
  // Retorna la creación recursiva
  return new_ls_from_string_recursive(input);
}

void LSS_print(struct LSS_Node *node) {
  // Comprobación de que exista al menos un nodo
  if(node == NULL) {
    printf("No hay datos");
    return;
  }
  int count = 0;
  print_recursive(node, true, &count);
}

void LSS_gen_dotfile(struct LSS_Node *lss, char *filename) {
  char *filename_ext = sdscatprintf(sdsempty(), "%s.dot", filename);
  printf("Generando archivo %s\n", filename_ext);
  // open file in write mode
  FILE *file = fopen(filename_ext, "w");
  if(file == NULL) {
    printf("<x> No se pudo crear el archivo\n");
    return;
  }
  fprintf(
    file,
    "digraph AF {\n\tgraph [center=true splines=true]\n\tnode[shape=record]\n"
  );
  int id = 0;
  gen_dot_nodes(file, lss, &id, NULL, 0, 0);
  fprintf(file, "}");
  fclose(file);
  printf("Archivo %s generado\n", filename_ext);
  sdsfree(filename_ext);
}

void LSS_free(struct LSS_Node ** lss) {
  struct LSS_Node *aux = *lss;
  // Exit si el nodo no existe
  if(aux == NULL) return;
  if(aux->type != STR) {
    // Se libera el subconjunto o sublista
    LSS_free(&(aux->data));
    // Se libera el siguiente elemento
    LSS_free(&(aux->next));
  } else {
    // Se libera la cadena
    sdsfree(aux->str);
    aux->str = NULL;
  }
  // Finalmente se libera el nodo actual
  free(*lss);
  *lss = NULL;
}

struct LSS_Node *LSS_get_elem_by_pos(struct LSS_Node **ls, int pos) {
  if(!*ls) return NULL;
  if((*ls)->type == STR) return NULL;
  // Para no modificar la variable fuera de la función
  struct LSS_Node *current_elem = *ls;
  int current_pos = 1;
  // Ciclo que se detiene al llegar a pos, o al último elemento
  while(current_pos < pos && current_elem != NULL) {
    current_elem = current_elem->next;
    current_pos++;
  }
  // Si existe el elemento, retornarlo
  if(current_elem) return current_elem->data;
  else return NULL;
}

int LSS_get_pos_by_elem(struct LSS_Node *ls, struct LSS_Node *element) {
  if(!ls) return -1;
  if(ls->type == STR) return -1;
  if(DEBUG) { printf("Buscando \"");LSS_print(element);printf("\"\n"); }
  int pos = 1;
  while(ls != NULL) {
    int comp = LSS_compare(ls->data, element);

    // En el caso de conjuntos, si el elemento es menor, no es necesario seguir
    // buscando
    // (Todavía no se puede asegurar que un conjunto esté siempre ordenado) // ???
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

char *LSS_get_elem_string(struct LSS_Node *ls) {
  if(!ls) return NULL;
  else if(ls->type == STR) return ls->str;
  else if(ls->data != NULL) return LSS_get_elem_string(ls->data);
  else return NULL;
}

enum LSS_NodeType  LSS_get_elem_type(struct LSS_Node *lss) {
  if(lss == NULL) return -1;
  else return lss->type;
}

// node1 > node2 return 1
// node1 < node2 return -1
// node1 = node2 return 0
int LSS_compare(struct LSS_Node *node1, struct LSS_Node *node2) {
  int returnValue = 0;
  int type1 = get_data_priority(node1);
  int type2 = get_data_priority(node2);
  // Comparación de prioridades
  if(type1 > type2) returnValue = 1;
  else if(type1 < type2) returnValue = -1;
  // Ambos son NULL
  else if(type1 == 0) returnValue = 0;
  // Comparación de cadenas
  else if(type1 == 1) {
    if(compare_strings(node1->str, node2->str) > 0) returnValue = 1;
    else if(compare_strings(node1->str, node2->str) < 0) returnValue = -1;
    else returnValue = 0;
    // Comparación de conjuntos y listas
  } else {
    int size1 = get_ls_size(node1);
    int size2 = get_ls_size(node2);
    if(size1 > size2) returnValue = 1;
    else if(size1 < size2) returnValue = -1;
    else {
      int resComp = LSS_compare(node1->data, node2->data);
      if(resComp > 0) returnValue = 1;
      else if(resComp < 0) returnValue = -1;
      else returnValue = LSS_compare(node1->next, node2->next);
    }
  }
  // DBG_log("<?> El resultado de \"%s\" es %d\n", __func__, returnValue);
  return returnValue;
}

struct LSS_Node *LSS_clone(struct LSS_Node *lss, enum LSS_NodeType type) {
  if(lss == NULL) return NULL;

  if(lss->type == STR && type != STR) {
    printf(
      "<x> No se puede convertir de forma directa un nodo STR en uno LST o SET."
      " Usa \"LSS_from_string\".\n"
    );
    return NULL;
  }

  struct LSS_Node *newNode;
  newNode = new_ls_node(type);
  if(newNode == NULL) {
    error_messages(MALLOC, __func__, __FILE__, __LINE__);
    return NULL;
  }
  if(type != STR) {
    if(lss->data != NULL) {
      newNode->data = LSS_clone(lss->data, lss->data->type);
    } else newNode->data = NULL;
    newNode->next = LSS_clone(lss->next, type);
  } else {
    newNode->str = lss_to_string(sdsempty(), lss, true);
  }
  return newNode;
}

// !#S

// #S CONJUNTOS ////////////////////////////////////////////////////////////////

int LSS_SET_cardinal(struct LSS_Node *node) {
  if(node->type != SET) return -1;
  else return get_ls_size(node);
}

struct LSS_Node *LSS_SET_new_empty(void) {
  struct LSS_Node *new = new_ls_node(SET);
  return new;
}

struct LSS_Node *LSS_SET_new_intersection(
  struct LSS_Node *setA,
  struct LSS_Node *setB
) {
  if(setA == NULL || setB == NULL) return NULL;
  struct LSS_Node *intersection = new_ls_node(SET);
  if(intersection == NULL) return NULL;

  while(setA != NULL && setB != NULL) {
    int comp = LSS_compare(setA->data, setB->data);
    // A[i] = B[j]
    if(comp == 0) {
      if(LSS_LST_SET_has_data(intersection)) {
        struct LSS_Node *setNode = new_ls_node(SET);
        if(setNode == NULL) {
          error_messages(MALLOC, __func__, __FILE__, __LINE__);
          return NULL;
        }
        setNode->data = LSS_clone(setA->data, setA->data->type);
        int res = add_node_in_order(&intersection, setNode);
        if(res <= 0) {
          LSS_free(&setNode);
          return NULL;
        }
      } else intersection->data = LSS_clone(setA->data, setA->data->type);
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

int LSS_SET_add_element(struct LSS_Node **setRoot, struct LSS_Node *node) {
  if((*setRoot) == NULL) {
    printf(
      "<x> El primer argumento de \"%s\" es NULL <%s:%d>\n",
      __func__, __FILE__, __LINE__
    );
    return -1;
  }
  if((*setRoot)->type != SET) {
    printf(
      "<x> El primer argumento de \"%s\" no es un conjunto <%s:%d>\n",
      __func__, __FILE__, __LINE__
    );
    return -1;
  }
  if(node == NULL) {
    printf(
      "<x> El segundo argumento de \"%s\" es NULL <%s:%d>\n",
      __func__, __FILE__, __LINE__
    );
    return -1;
  }
  if(LSS_LST_SET_has_data(*setRoot)) {
    struct LSS_Node *setNode = new_ls_node(SET);
    if(setNode == NULL) {
      error_messages(MALLOC, __func__, __FILE__, __LINE__);
      return -1;
    }
    setNode->data = node;
    int res = add_node_in_order(setRoot, setNode);
    if(res <= 0) free(setNode);
    return res;
  } else {
    (*setRoot)->data = node;
    return 1;
  }
}

// !#S

// #S LISTAS ///////////////////////////////////////////////////////////////////

struct LSS_Node *LSS_LST_new_empty(void) {
  struct LSS_Node * new = new_ls_node(LST);
  return new;
}

bool LSS_LST_append_element(struct LSS_Node **list, struct LSS_Node *node) {
  if((*list) == NULL) {
    printf(
      "<x> El primer argumento de \"%s\" es NULL <%s:%d>\n",
      __func__, __FILE__, __LINE__
    );
    return false;
  }
  if((*list)->type != LST) {
    printf(
      "<x> El primer argumento de \"%s\" no es una lista <%s:%d>\n",
      __func__, __FILE__, __LINE__
    );
    return false;
  }
  if(node == NULL) {
    printf(
      "<x> El segundo argumento de \"%s\" es NULL <%s:%d>\n",
      __func__, __FILE__, __LINE__
    );
    return false;
  }
  if(LSS_LST_SET_has_data(*list)) {
    struct LSS_Node *lstNode = new_ls_node(LST);
    if(lstNode == NULL) {
      error_messages(MALLOC, __func__, __FILE__, __LINE__);
      return false;
    }
    link_node_data(lstNode, node);
    link_node_next(get_last_node(*list), lstNode);
  } else link_node_data(*list, node);
  return true;
}

int LSS_LST_size(struct LSS_Node *list) {
  if(list->type != LST) return -1;
  return get_ls_size(list);
}

// !#S:!#S

////////////////////////////////////////////////////////////////////////////////
// #S DEFINICIONES PRIVADAS ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// #S GENERAL //////////////////////////////////////////////////////////////////

void error_messages(enum Err key, const char *func, char * file, int line) {
  switch(key) {
    case MALLOC:
      printf(
        "<x> No se pudo reservar la memoria necesaria <%s><%s:%d>\n",
        func, file, line
      );
      break;
    case NOT_SET:
      printf("<x> Falta una o mas llaves <%s><%s:%d>\n", func, file, line);
      break;
    case NOT_LST:
      printf("<x> Falta uno o mas corchetes <%s><%s:%d>\n", func, file, line);
      break;
    case NOT_LS:
      printf(
        "<x> La entrada no es un conjunto ({ }) ni una lista ([ ]) "
        "<%s><%s:%d>\n", func, file, line
      );
      break;
    default:
      printf("<x> Codigo de error desconocido <%s><%s:%d>\n", func, file, line);
      break;
  }
}

struct LSS_Node *new_ls_from_string_recursive(char *string) {
  // Crear raíz
  struct LSS_Node *rootNode = NULL;
  // De qué tipo es string
  enum LSS_NodeType stringType = get_string_type(string);

  // Se podría combinar esta función y 'split_string' en una?

  char **elementsArray;
  int elementsCount = 0;
  // Crear un array con los elementos de string
  elementsArray = split_string(string, &elementsCount);

  if(elementsCount > 0) { // Hay elementos
    for(int index = 0; index < elementsCount; index++) { // Por cada elemento
      // Nodo para el nuevo elemento, todos del mismo tipo
      struct LSS_Node *newNode = new_ls_node(stringType);
      // El elemento es un subconjunto/sublista
      if(get_string_type(elementsArray[index]) != STR) {
        // Crear el subset/sublst y unirlo como elemento del actual (como dato)
        link_node_data(
          newNode,
          new_ls_from_string_recursive(elementsArray[index])
        );
      } else if(
        // El elemento es STR. Si la cadena no es vacía, crear el nodo STR y
        // unirlo como elemento (como dato)
        sdscmp(elementsArray[index], "") != 0
      ) {
        link_node_data(newNode, new_str_node(elementsArray[index]));
      }

      // Si raíz aún no apunta a un nodo, inicializar
      if(rootNode == NULL) rootNode = newNode;
      else if(stringType != SET) {
        // Enlazar newNode como el ultimo elemento del árbol
        link_node_next(get_last_node(rootNode), newNode);
      } else {
        // Insertar el nuevo nodo de forma ordenada solo cuando es elemento de
        // un conjunto
        int res = add_node_in_order(&rootNode, newNode);
        if(res <= 0) {
          LSS_free(&newNode);
        }
      }
    } // <- Fin del ciclo
  }
  // Libera la memoria utilizada por el array
  sdsfreesplitres(elementsArray, elementsCount);
  return rootNode;
}

char *lss_to_string(char *string, struct LSS_Node * nodo, bool primero) {
  // Caso del conjunto vacío o lista vacía
  if(nodo == NULL) return string;

  if(nodo->type == STR) {
    // Se muestra la cadena 

    return sdscat(string, nodo->str);;
  }
  // Se abre el conjunto o lista
  if(primero) {
    string = (nodo->type == SET) ? sdscat(string, "{") : sdscat(string, "[");
  }
  // Se muestra el dato
  string = lss_to_string(string, nodo->data, true);
  if(nodo->next == NULL) {
    // Se cierra el conjunto o lista
    return (nodo->type == SET) ? sdscat(string, "}") : sdscat(string, "]");
  }
  // Se muestra el siguiente elemento
  string = sdscat(string, ",");
  return lss_to_string(string, nodo->next, false);
}

struct LSS_Node *new_ls_node(enum LSS_NodeType type) {
  struct LSS_Node *nuevo;
  nuevo = malloc(sizeof(*nuevo));
  nuevo->type = type;
  nuevo->data = NULL;
  nuevo->next = NULL;
  return nuevo;
}

struct LSS_Node *new_str_node(char *str) {
  struct LSS_Node *nuevo;
  nuevo = malloc(sizeof(*nuevo));
  nuevo->type = STR;
  nuevo->str = sdsnew(str);
  return nuevo;
}

void link_node_data(struct LSS_Node *parent, struct LSS_Node *data) {
  (*parent).data = data;
}

void link_node_next(struct LSS_Node *parent, struct LSS_Node *next) {
  parent->next = next;
}

struct LSS_Node *get_last_node(struct LSS_Node* primero) {
  if(primero->next != NULL) {
    return get_last_node(primero->next);
  } else return primero;
}

bool is_string_a_valid_lss(char *str) {
  if(str == NULL) return false;
  struct StackNode *stack = NULL;
  char match;
  char char_str[2] = "\0"; // string temporal para el push
  char * temp_str; // stirng temporal para el pop
  int i = 0;

  while(str[i] != '\0') {
    match = '\0';
    switch(str[i]) {
      case '[': case '{':
        char_str[0] = str[i];
        push(&stack, char_str);
        break;
      case ']': match = '['; break;
      case '}': match = '{'; break;
    }
    if(match != '\0') {
      temp_str = pop(&stack);
      if(temp_str[0] != match) {
        error_messages(
          match == ']' ? NOT_LST : NOT_SET,
          __func__, __FILE__, __LINE__
        );
        sdsfree(temp_str);
        return false;
      }
    }
    i++;
  }
  if(is_empty(stack)) return true;
  error_messages(NOT_LS, __func__, __FILE__, __LINE__);
  return false;
}

enum LSS_NodeType get_string_type(char *str) {
  if(str == NULL) return STR;
  switch(str[0]) {
    case '{':
      if(is_string_a_valid_lss(str)) {
        DBG_log("<?> Tipo SET detectado ('{')\n");
        return SET;
      }
      break;
    case '[':
      if(is_string_a_valid_lss(str)) {
        DBG_log("<?> Tipo LST detectado ('[')\n");
        return LST;
      }
      break;
  }
  DBG_log("<?> Tipo STR detectado ('%c')\n", str[0]);
  return STR;
}

// Separa str en un array donde cada posición es un elemento
char **split_string(char *str, int *cant) {
  char **array; // Array
  int lar = strlen(str); // Largo de la cadena str
  int elem = 0; // Cantidad de elementos del array
  int mem = 2; // Cantidad de "espacios" que se añaden al asignar memoria
  int ini = 0; // Inicio de cada elemento de str
  int i = 0; // Posición del carácter de str siendo analizado

  int n_set = 0; // Contador de conjuntos "abiertos"
  int n_lst = 0; // Contador de listas "abiertas"

  // Se decide cómo proceder
  int req_set = 0; // Suponiendo que str no es un conjunto
  int req_lst = 0; // Suponiendo que str no es una lista

  // Se confirma si str es un conjunto o una lista
  if(is_string_a_valid_lss(str)) {
    ini = 1; // Para ignorar el carácter de apertura
    lar = lar - 1; // Para ignorar el carácter de cierre
    switch(get_string_type(str)) {
      // n_set debe ser 1 y n_lst debe ser 0 para tener en cuenta una coma
      case SET: req_set = 1; req_lst = 0; break;
        // n_set debe ser 0 y n_lst debe ser 1 para tener en cuenta una coma
      case LST: req_set = 0; req_lst = 1; break;
        // Caso inalcanzable debido a "is_string_a_list_or_set" (?)
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
        sdstrim(array[elem], " \"");
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
  sdstrim(array[elem], " \"");
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
void print_recursive(struct LSS_Node * nodo, bool primero, int *count) {
  // hardlimit por si el LSS tiene algún error
  if(*count >= 100) {
    printf(" <x> Limite alcanzado ");
    return;
  }
  // Caso del conjunto vacío o lista vacía
  if(nodo == NULL) return;

  enum LSS_NodeType type = nodo->type;
  if(type == STR) {
    // Se muestra la cadena
    printf("\"%s\"", nodo->str);
    return;
  }
  // Se abre el conjunto o lista
  if(primero) {
    (type == SET) ? printf("{") : printf("[");
  }
  // Se muestra el dato
  *count = *count + 1;
  print_recursive(nodo->data, true, count);
  if(nodo->next == NULL) {
    // Se cierra el conjunto o lista
    type == SET ? printf("}") : printf("]");
    return;
  }
  // Se muestra el siguiente elemento
  printf(",");
  *count = *count + 1;
  print_recursive(nodo->next, false, count);
}

bool LSS_LST_SET_has_data(struct LSS_Node * sl) {
  if(sl == NULL) return false;
  if(sl->type == STR) return true;
  if(sl->data != NULL) return true;
  else {
    DBG_log("<?> Nodo LST/SET sin dato detectado\n");
    return false;
  }
}

bool is_string_a_number(char * string) {
  bool isDecimal = false;
  bool isNumber = true;
  int index = 0;
  // ASCII , - . / 0 1 2 3 4 5 6 7 8 9 :
  while(string[index] != '\0' && isNumber == true) {
    // Posee caracteres válidos?
    if(
      string[index] < '-'
      || string[index] > '9'
      || string[index] == '/'
    ) isNumber = false;
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

int get_ls_size(struct LSS_Node *node) {
  // Control nodo válido
  if(node == NULL) return -1;

  if(node->type == STR) return -1;
  // Control conjunto vacío
  if(!LSS_LST_SET_has_data(node)) return 0;

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
int compare_strings(char *string1, char *string2) {
  // strnatcmp no tiene en cuenta números negativos por lo que se realiza una
  // comparación simple si alguna de las cadenas es un número negativo
  if(is_string_a_number(string1) && is_string_a_number(string2)) {
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

void gen_dot_nodes(
  FILE *file,
  struct LSS_Node *current,
  int *id,
  struct LSS_Node *prev,
  int id_prev,
  int data_or_next
) {
  int aux = *id;
  // Caso del conjunto vacío o lista vacía
  if(current == NULL) {
    fprintf(file, "\tnodo%d[label=\"NULL\" shape=none]\n", *id);
    if(prev != NULL) {
      if(data_or_next == 0) {
        fprintf(file, "\tnodo%d:data->nodo%d\n", id_prev, *id);
      } else fprintf(file, "\tnodo%d:next->nodo%d\n", id_prev, *id);
    }
    *id = *id + 1;
    return;
  }
  enum LSS_NodeType type = current->type;
  if(type == STR) {
    // Se muestra la cadena
    if(current->str[0] == '{') {
      char *temp = sdsnewlen(current->str + 1, strlen(current->str) - 2);
      fprintf(
        file,
        "\tnodo%d[label=\"<type>STR|<str>\\\"\\{%s\\}\\\"\"]\n",
        *id,
        temp
      );
      sdsfree(temp);
    } else fprintf(
      file,
      "\tnodo%d[label=\"<type>STR|<str>\\\"%s\\\"\"]\n",
      *id,
      current->str
    );
    if(prev != NULL) {
      if(data_or_next == 0) {
        fprintf(file, "\tnodo%d:data->nodo%d:type\n", id_prev, *id);
      } else fprintf(file, "\tnodo%d:next->nodo%d:type\n", id_prev, *id);
    }
    *id = *id + 1;
    return;
  } else if(type == SET) {
    fprintf(
      file,
      "\tnodo%d[label=\"<type>SET|<data>data|<next>next\" color=blue]\n",
      *id
    );
    if(prev != NULL) {
      if(data_or_next == 0) {
        fprintf(file, "\tnodo%d:data->nodo%d:type\n", id_prev, *id);
      } else fprintf(file, "\tnodo%d:next->nodo%d:type\n", id_prev, *id);
    }
    *id = *id + 1;
  } else {
    fprintf(
      file,
      "\tnodo%d[label=\"<type>LST|<data>data|<next>next\" color=green]\n",
      *id
    );
    if(prev != NULL) {
      if(data_or_next == 0) {
        fprintf(file, "\tnodo%d:data->nodo%d:type\n", id_prev, *id);
      } else fprintf(file, "\tnodo%d:next->nodo%d:type\n", id_prev, *id);
    }
    *id = *id + 1;
  }
  // Se muestra el dato
  gen_dot_nodes(file, current->data, id, current, aux, 0);
  gen_dot_nodes(file, current->next, id, current, aux, 1);
}

// !#S

// #S CONJUNTOS ////////////////////////////////////////////////////////////////

int get_data_priority(struct LSS_Node * data) {
  if(data == NULL) return 0; // El nodo existe
  int dataType;
  if(data->type == STR) dataType = 1;
  else dataType = (data->type == SET) ? 2 : 3;
  DBG_log("<?> La funcion \"%s\" retorna %d\n", __func__, dataType);
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
int add_node_in_order(struct LSS_Node **firstNode, struct LSS_Node *newNode) {
  if(*firstNode == NULL) {
    printf(
      "<x> El primer argumento de \"%s\" debe ser un conjunto <%s:%d>\n",
      __func__, __FILE__, __LINE__
    );
    return -1;
  }
  if((*firstNode)->type != SET) {
    printf(
      "<x> El primer argumento de \"%s\" debe ser un conjunto <%s:%d>\n",
      __func__, __FILE__, __LINE__
    );
    return -1;
  }
  struct LSS_Node *currentNode = *firstNode;
  struct LSS_Node *previousNode = *firstNode;
  int current_pos = 1;
  int cmpNodes = LSS_compare(newNode->data, currentNode->data);
  // Búsqueda de la posición
  while(cmpNodes > 0 && currentNode->next != NULL) {
    DBG_log(
      "<?> \"%s\" es mayor que \"%s\"\n",
      newNode->data->str,
      currentNode->data->str
    );
    previousNode = currentNode;
    currentNode = currentNode->next;
    current_pos++;
    cmpNodes = LSS_compare(newNode->data, currentNode->data);
  }
  if(cmpNodes == 0) {
    if(DEBUG) {
      printf("<?> Elemento repetido \"");
      LSS_print(newNode->data);
      printf("\"\n");
    }
    return 0;
  }
  // Caso especial para la primera posición (raíz)
  if(current_pos == 1) {
    if(cmpNodes < 0) {
      newNode->next = *firstNode;
      *firstNode = newNode;
    } else {
      newNode->next = (*firstNode)->next;
      (*firstNode)->next = newNode;
      current_pos++;
    }
  } else if(cmpNodes < 0) {
    newNode->next = currentNode;
    previousNode->next = newNode;
  } else {
    newNode->next = currentNode->next;
    currentNode->next = newNode;
    current_pos++;
  }
  if(DEBUG) {
    printf("<?> Elemento \"");
    LSS_print(newNode->data);
    printf("\" insertado en la posicion %d\n", current_pos);
  }
  return current_pos;
}

// !#S

// #S LISTAS ///////////////////////////////////////////////////////////////////



// !#S

// #S CADENAS //////////////////////////////////////////////////////////////////



// !#S:!#S
