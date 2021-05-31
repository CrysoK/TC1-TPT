#include "set-lst.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "auxlib.h"

// DECLARACIONES PRIVADAS //////////////////////////////////////////////////////

// VARIABLES-CONSTANTES

// Codigos de error
enum ERR {
  // Error en asignación de memoria
  MALLOC,
  // Faltan llaves en la entrada
  NO_SET,
  // Faltan corchetes en la entrada
  NO_LST,
  // El primer o último caracter no es una llave o corchete
  NO_S_L
};

// PROTOTIPOS

// Mensajes de error
void            error(enum ERR codigo, char * archivo, int linea);
//
struct sl_nodo *sl_crear(char *);
// 
struct sl_nodo *sl_nodo_crear(enum TIPO_NODO);
//
void            sl_mostrar_rec(struct sl_nodo *);
//
struct sl_nodo *str_crear(char *);
//
void            link_dato(struct sl_nodo *, struct sl_nodo *);
//
void            link_sig(struct sl_nodo *, struct sl_nodo *);
// 
struct sl_nodo *ultimo_nodo(struct sl_nodo*);
// Extrae la cadena encerrada en llaves o corchetes
char          **str_separar(char *, int *);
// Quita espacios alrededor de un elemento
void            str_espacios(char *);
// Verifica que la entrada sea un conjunto o una lista
bool            str_valida(char *);
// Devuelve si la entrada es un conjunto, una lista o una cadena
enum TIPO_NODO  str_tipo(char *);
// 
bool            str_es_sl(char *);
//
bool            set_is_vacio(struct sl_nodo *);

// DEFINICIONES PÚBLICAS ///////////////////////////////////////////////////////

// GENERAL

struct sl_nodo *sl_nuevo(char *entrada) {
  // Validacion de la cadena de entrada
  if(!str_valida(entrada)) return NULL;
  // Puntero a retornar
  struct sl_nodo *nuevo = NULL;

  nuevo = sl_crear(entrada);

  return nuevo;
}

struct sl_nodo *sl_crear(char *input) {
  // Crear raiz
  struct sl_nodo *raiz = NULL;
  // De qué tipo es input
  enum TIPO_NODO tipo = str_tipo(input);

  char **array; int cant = 0;
  // Crear un array con los elementos de input
  array = str_separar(input, &cant);

  if(cant > 0) { // Hay elementos
    for(int i = 0; i < cant; i++) { // Por cada elemento
      struct sl_nodo *aux = sl_nodo_crear(tipo); // Nodo para el nuevo elemento, todos del mismo tipo
      if(str_tipo(array[i]) != STR) { // El elemento es un subconjunto/sublista
        // Crear el subset/sublst y unirlo como elemento del actual (como dato)
        link_dato(aux, sl_crear(array[i]));
      } else { // El elemento es STR
        // Crear el nodo STR y unirlo como elemento (como dato)
        link_dato(aux, str_crear(array[i]));
      }
      if(raiz == NULL) raiz = aux; // Si raiz aún no apunta a un nodo, inicializar
      else link_sig(ultimo_nodo(raiz), aux); // Enlazar aux como el ultimo elemento del arbol
    } // Fin del ciclo
  } else { // No hay elementos
    // ERROR: Cantidad de elementos = 0 ???
  }
  //sdsfreesplitres(input, cant); // Libera la memoria utilizada por el array (Provoca errores)
  return raiz;
}

void sl_mostrar(struct sl_nodo * nodo) {
  sl_mostrar_rec(nodo);
  printf("\n");
}

// CONJUNTOS

int set_cardinal(struct sl_nodo *nodo) {
  // Control conjunto vacío
  if(set_is_vacio(nodo)) return 0;

  int cnt = 0;
  while(nodo != NULL) {
    cnt++;
    nodo = nodo->sig;
  }
  return cnt;
}

// LISTAS

struct sl_nodo *lst_vacia() {
  struct sl_nodo * nuevo = sl_crear(sdsnew("[]"));
  return nuevo;
}

void lst_ins_final(struct sl_nodo ** lista, char * nuevo) {
  if((*lista)->tipo != LST) {
    // ERROR
    printf("<x> El argumento no es una lista\n");
    return;
  }

  if(str_es_sl(nuevo)) {
    if(DEBUG) printf("<?> Insertando un conjunto o lista a la lista\n");
    link_sig(ultimo_nodo(*lista), sl_nuevo(nuevo));
  } else {
    if(DEBUG) printf("<?> Insertando una cadena a la lista\n");
    struct sl_nodo *aux = sl_nodo_crear(LST);
    link_dato(aux, str_crear(nuevo));
    link_sig(ultimo_nodo(*lista), aux);
  }
}

// DEFINICIONES PRIVADAS ///////////////////////////////////////////////////////

// GENERAL

void error(enum ERR key, char * f, int l) {
  switch(key) {
    case MALLOC:
      printf("<x> No se pudo reservar la memoria necesaria <%s:%d>\n", f, l);
      break;
    case NO_SET:
      printf("<x> Falta una o mas llaves <%s:%d>\n", f, l);
      break;
    case NO_LST:
      printf("<x> Falta uno o mas corchetes <%s:%d>\n", f, l);
      break;
    case NO_S_L:
      printf("<x> La entrada no es un conjunto ({ }) ni una lista ([ ]) <%s:%d>\n", f, l);
      break;
    default:
      printf("<x> Codigo de error desconocido <%s:%d>\n", f, l);
      break;
  }
}

struct sl_nodo *sl_nodo_crear(enum TIPO_NODO tipo) {
  struct sl_nodo *nuevo;
  nuevo = malloc(sizeof * nuevo);
  nuevo->tipo = tipo;
  nuevo->dato = NULL;
  nuevo->sig = NULL;
  return nuevo;
}

struct sl_nodo *str_crear(char *str) {
  struct sl_nodo *nuevo;
  // nuevo = (struct sl_nodo *)malloc(sizeof(struct sl_nodo));
  nuevo = malloc(sizeof * nuevo);
  nuevo->tipo = STR;
  nuevo->str = sdsnew(str);
  return nuevo;
}

void link_dato(struct sl_nodo *parent, struct sl_nodo *dato) {
  (*parent).dato = dato;
}

void link_sig(struct sl_nodo *parent, struct sl_nodo *sig) {
  parent->sig = sig;
}

struct sl_nodo *ultimo_nodo(struct sl_nodo* primero) {
  if(primero->sig != NULL) {
    return ultimo_nodo(primero->sig);
  } else return primero;
}

void str_espacios(char *str) {
  sdstrim(str, " ");
}

bool str_valida(char *str) {
  bool resp = true;
  size_t len = sdslen(str);
  // Control primer caracter
  if(str[0] == '{' || str[0] == '[') {
    // Control último caracter
    if(str[len - 1] == '}' || str[len - 1] == ']') {
      int n_set = 0;
      int n_lst = 0;
      // Control balance de {} y []
      for(int i = 0; i < len; i++) {
        switch(str[i]) {
          case '{': n_set++; break;
          case '}': n_set--; break;
          case '[': n_lst++; break;
          case ']': n_lst--; break;
        }
      }
      if(n_set != 0) { error(NO_SET, __FILE__, __LINE__); resp = false; }
      if(n_lst != 0) { error(NO_LST, __FILE__, __LINE__); resp = false; }
    } else {
      error(NO_S_L, __FILE__, __LINE__);
      resp = false;
    }
  } else {
    error(NO_S_L, __FILE__, __LINE__);
    resp = false;
  }
  return resp;
}

enum TIPO_NODO str_tipo(char *str) {
  switch(str[0]) {
    case '{':
      if(DEBUG) printf("<?> Tipo \"conjunto\" detectado ('{')\n");
      return SET;
    case '[':
      if(DEBUG) printf("<?> Tipo \"lista\" detectado ('[')\n");
      return LST;
    default:
      if(DEBUG) printf("<?> Tipo \"cadena\" detectado ('%c')\n", str[0]);
      return STR;
  }
}

/* Retorna true si str es un conjunto o una cadena, comprobando si el ultimo
 * caracter es el cierre del primero, usando stack*/
bool str_es_sl(char *str) {
  int cnt = 0; // Se crea el contador
  char abre, cierra; // Caracter de apertura y cierre
  switch(str_tipo(str)) {
    case STR: return false; break; // No es conjunto ni lista
    // Es conjunto, se usan los caracteres correspondientes
    case SET: abre = '{'; cierra = '}'; break;
      // Es lista, se usan los caracteres correspondientes
    case LST: abre = '['; cierra = ']'; break;
  }
  // Se analiza caracter por caracter
  for(int i = 0; i < sdslen(str); i++) {
    // Si el caracter es el de apertura, se incrementa el contador
    if(str[i] == abre) cnt++;
    // Si el caracter es el de cierre:
    else if(str[i] == cierra) {
      // Se decrementa el contador
      cnt--;
      // Si el contador es cero:
      if(cnt == 0) {
        // Si es el último caracter, entonces es un conjunto/lista
        if(i == sdslen(str) - 1) return true; // El char de cierre esta al final
        // Si no es el último, el conjunto "cerró" antes del final y solo es un elemento de str
        else return false; // El char de cierre no está al final
      }
    }
  }
  return false; // ERROR: Faltan llaves o corchetes
}

// Separa str en un array donde cada posición es un elemento
char **str_separar(char *str, int *cant) {
  char **array; // Array
  int lar = sdslen(str); // Largo de la cadena str
  int elem = 0; // Cantidad de elementos del array
  int mem = 2; // Cantidad de "espacios" que se añaden al asignar memoria
  int ini = 0; // Inicio de cada elemento de str
  int i = 0; // Posición del caracter de str siendo analizado

  int n_set = 0; // Contador de conjuntos "abiertos"
  int n_lst = 0; // Contador de listas "abiertas"

  // Se decide cómo proceder
  int req_set = 0; // Suponiendo que str no es un conjunto
  int req_lst = 0; // Suponiendo que str no es una lista

  if(str_es_sl(str)) { // Se confirma que str sí es un conjunto o una lista
    ini = 1; // Para ignorar el caracter de apertura
    lar = lar - 1; // Para ignorar el caracter de cierre
    switch(str_tipo(str)) {
      // n_set debe ser 1 y n_lst debe ser 0 para tener en cuenta una coma
      case SET: req_set = 1; req_lst = 0; break;
        // n_set debe ser 0 y n_lst debe ser 1 para tener en cuenta una coma
      case LST: req_set = 0; req_lst = 1; break;
        // Caso inalcanzable debido a "str_es_sl" (?)
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

void sl_mostrar_rec(struct sl_nodo * nodo) {
  if(nodo != NULL) { // Comprobación de que exista al menos un nodo
    enum TIPO_NODO tipo = nodo->tipo;
    (tipo == SET) ? printf("{") : printf("[");
    while(nodo != NULL) {
      struct sl_nodo *aux = nodo;
      if(aux->dato->tipo != STR) {
        sl_mostrar_rec(aux->dato);
      } else {
        printf("%s", aux->dato->str);
      }
      if(nodo->sig != NULL) printf(",");
      nodo = nodo->sig;
    }
    tipo == SET ? printf("}") : printf("]");
  } else printf("<i> No hay datos");
}

// CONJUNTOS

bool set_is_vacio(struct sl_nodo * set) {
  if(set->dato->tipo != STR) return false;
  else if(sdscmp(set->dato->str, "") == 0) {
    if(DEBUG) printf("<?> Conjunto vacio detectado\n");
    return true;
  } else return false;
}
