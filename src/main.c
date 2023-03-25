// INFORMACIÓN /////////////////////////////////////////////////////////////////

/*\
 * TRABAJO PRÁCTICO TRANSVERSAL - TEORÍA DE LA COMPUTACIÓN I
 *
 * PARTE 1
 * 1) Cargar un autómata finito "por partes": primero el conjunto de estados,
 * luego el alfabeto y así (la transición de estados cargarla por terna).
 * 2) Crear un árbol que almacene al autómata finito ingresado.
 * 3) Mostrar el autómata finito.
 *
 * PARTE 2
 * 4) Dada una cadena, indicar si pertenece al lenguaje regular cuyo autómata
 * finito es el ingresado.
 * 5) Si el autómata finito es no determinista, convertirlo en determinista.
 *
 * NOTAS
 * - No usar caracteres que no formen parte de ASCII estándar
 *
 * REFERENCIA
 * <x> Error
 * <!> Advertencia
 * <i> Información
 * <?> Debug
 *  #  Input
 *
 * FUENTES
 * - SDS Simple Dynamic Strings: https://github.com/jcorporation/sds
 * - natsort: https://github.com/sourcefrog/natsort
\*/

// PRECOMPILADOR ///////////////////////////////////////////////////////////////

#include <stdio.h> // Entrada y salida estándar
#include <stdlib.h> // Librería general estándar
#include <string.h> // Manejo de cadenas estándar
#include <stdbool.h> // Tipo de dato booleano estándar

#include "include/LSS.h" // TAD "Lists, sets & strings"
#include "include/sds/sds.h" // Librería externa "Simple Dynamic Strings"
#include "include/debug.h"

bool DEBUG = false;

// DECLARACIONES ///////////////////////////////////////////////////////////////

// AF

/* NONE = Ninguno
 * AFND = Automata Finito No Determinista
 * AFD = Automata Finito Determinista */
enum AF_Type { NONE, AFND, AFD };
/* Q = Conjunto de estados
 * E = Conjunto de símbolos
 * D = Conjunto de transiciones
 * q0 = Estado inicial
 * F = Conjunto de estados de aceptación */
enum AF_Elem { Q = 1, E, D, q0, F };
/* Devuelve un conjunto con los estados alcanzados a partir de los estados del
 * conjunto 'states'.
 * 'delta' = conjunto de transiciones a utilizar
 * 'states' = conjunto de estados de partida
 * 'symbol' = símbolo de entrada */
struct LSS_Node *AF_get_transition(
  struct LSS_Node *delta,
  struct LSS_Node *states,
  char *symbol
);
/* Divide una cadena, según los símbolos indicados, en elementos de un array. La
 * cadena original no se modifica.
 * 'string' = cadena a dividir
 * 'symbols' = conjunto de símbolos
 * 'size' = cantidad de elementos resultantes */
char **AF_split_string_by_symbols(
  char *string,
  struct LSS_Node *symbols,
  int *size
);
/* Indica si una cadena es aceptada por un autómata finito */
bool AF_is_string_accepted(struct LSS_Node *af, char *string);
/* Validación simple de un autómata finito. Devuelve el tipo del mismo. */
enum AF_Type AF_get_type(struct LSS_Node *af);
/* Convierte un autómata finito no determinista en uno determinista. No elimina
 * el original. */
struct LSS_Node *AF_afnd_to_afd(struct LSS_Node *afnd);

// Cadenas

/* Recibe la entrada desde consola y se almacena dinámicamente. La memoria debe
 * ser liberada con 'sdsfree' */
char *SDS_new_user_input(void);
/* Elimina el contenido de una cadena sin liberar su memoria asignada */
void SDS_clear(char * str);

// Interfaz y otras

/* Menú principal. */
int UI_main_menu(enum AF_Type af_type);
/* Interfaz para ingresar un autómata finito */
struct LSS_Node *UI_new_af(void);
/* Interfaz para mostrar un autómata finito */
void UI_print_af(struct LSS_Node *automata_finito);
//
void UI_acceptance_menu(struct LSS_Node *automata_finito);
// Creación del gráfico del autómata finito
void UI_graphviz_menu(struct LSS_Node *automata_finito);
//
int UI_new_user_option(int min, int max);
// Ejecuta un comando para limpiar la consola
void UI_clear_console(void);
// Pide presionar ENTER para continuar
void UI_pause(void);
// Elimina el buffer de stdin
void UI_clear_stdin(void);

// MAIN ////////////////////////////////////////////////////////////////////////

int main(void) {
  DBG_log("<?> Mensajes de depuracion activados\n");
  struct LSS_Node *automata_finito = NULL;
  struct LSS_Node *aux = NULL;
  enum AF_Type af_type = NONE;
  int user_option;
  do { // Menu principal
    user_option = UI_main_menu(af_type);
    switch(user_option) {
      case 1:
        LSS_free(&automata_finito);
        automata_finito = UI_new_af();
        af_type = AF_get_type(automata_finito);
        break;
      case 2:
        UI_print_af(automata_finito);
        UI_pause();
        break;
      case 3:
        UI_graphviz_menu(automata_finito);
        break;
      case 4:
        UI_acceptance_menu(automata_finito);
        UI_pause();
        break;
      case 5:
        aux = AF_afnd_to_afd(automata_finito);
        if(aux != NULL) {
          automata_finito = aux;
          printf("La conversion ha sido realizada con exito\n");
        } else printf("La conversion no pudo ser realizada\n");
        af_type = AF_get_type(automata_finito);
        UI_pause();
        break;
      case 0:
        break;
    }
  } while(user_option != 0);
  LSS_free(&automata_finito);
  return 0;
}

// DEFINICIONES ////////////////////////////////////////////////////////////////

// AF

char **AF_split_string_by_symbols(
  char *string,
  struct LSS_Node *symbols_set,
  int *array_size
) {
  // Si no hay símbolos
  if(!LSS_LST_SET_has_data(symbols_set)) {
    printf("<x> El conjunto de simbolos no contiene elementos\n");
    return NULL;
  }
  // Si la cadena no existe
  if(string == NULL) {
    printf("<x> La cadena a evaluar es NULL\n");
    return NULL;
  }
  // Array de cadenas
  char **array;
  // Cantidad de símbolos del AF
  int set_size = LSS_SET_cardinal(symbols_set);
  // Cantidad de memoria asignada
  int mem_block = 2;
  // Posición en el array (y cantidad)
  int elem = 0;
  // Inicio de la subcadena (símbolo)
  int start = 0;
  // Carácter de la cadena principal
  int i = 0;
  // Símbolo del conjunto de símbolos del AF
  int j = 1;
  // Carácter de un símbolo del AF
  int k = 0;

  array = malloc(sizeof(*array) * mem_block);
  if(array == NULL) {
    printf("<x> Error en la asignacion de memoria dinamica\n");
    goto error;
  }
  // Caso de cadena NO vacía
  while(string[i] != '\0') {
    // Es necesario mas memoria?
    if(mem_block < elem + 2) {
      mem_block *= 2;
      char **aux = realloc(array, sizeof(*array) * mem_block);
      if(aux == NULL) {
        printf("<x> Error en la asignacion de memoria dinamica\n");
        goto error;
      }
      array = aux;
    }
    // Se obtiene el símbolo del conjunto
    char *af_symbol = LSS_get_elem_string(LSS_get_elem_by_pos(&symbols_set, j));
    int sym_length = strlen(af_symbol);
    // Mientras los caracteres sean iguales
    while(
      af_symbol[k] == string[i]
      && af_symbol[k] != '\0'
      && string[i] != '\0'
    ) {
      k++; // Siguiente carácter del símbolo
      i++; // Siguiente carácter de la cadena
    }
    if(k == sym_length) {
      // Extraer símbolo
      array[elem] = sdsnewlen(string + start, sym_length);
      elem++;
      start = i;
      j = 1;
    } else {
      if(j == set_size) {
        printf(
          "<x> La cadena tiene simbolos que no estan definidos en el AF\n"
        );
        goto error;
      }
      j++; // Siguiente símbolo
    }
    i = start;
    k = 0;
  }
  // Caso de cadena "vacía"
  if(i == 0 && string[0] == '\0') {
    array[0] = sdsnew("");
    elem = 1;
  }

  *array_size = elem;
  return array;

  error: // En caso de que algo falle se libera la memoria asignada
  for(j = 0; j < elem; j++) sdsfree(array[j]);
  free(array);
  *array_size = 0;
  return NULL;
}

// Se asume que el AF es correcto (validar antes)
bool AF_is_string_accepted(struct LSS_Node *af, char *string) {
  struct LSS_Node *start_state = LSS_get_elem_by_pos(&af, q0);
  if(start_state == NULL) return false;
  struct LSS_Node *symbol_set = LSS_get_elem_by_pos(&af, E);
  if(symbol_set == NULL) return false;
  struct LSS_Node *delta_set = LSS_get_elem_by_pos(&af, D);
  if(delta_set == NULL) return false;
  struct LSS_Node *final_set = LSS_get_elem_by_pos(&af, F);
  if(final_set == NULL) return false;

  struct LSS_Node *states_set = NULL;
  struct LSS_Node *intersection = NULL;
  struct LSS_Node *aux_node = NULL;
  int aux_num;

  //
  int num_of_symbols = 0;
  char **string_symbols = AF_split_string_by_symbols(
    string,
    symbol_set,
    &num_of_symbols
  );
  if(string_symbols == NULL || num_of_symbols == 0) return false;

  if(DEBUG) {
    for(int a = 0; a < num_of_symbols; a++) printf("%s ", string_symbols[a]);
    printf("\n");
  }

  states_set = LSS_SET_new_empty();
  if(states_set == NULL) {
    printf("<x> Error en la asignacion de memoria dinamica\n");
    goto error;
  }

  // Estado inicial
  aux_node = LSS_clone(start_state, LSS_get_elem_type(start_state));
  aux_num = LSS_SET_add_element(&states_set, aux_node);
  if(aux_num <= 0) {
    if(aux_num < 0) {
      printf("<x> Error al insertar un elemento al conjunto\n");
      goto error;
    }
    LSS_free(&aux_node);
  }

  if(DEBUG) { printf("E. Inicial = "); LSS_print(states_set); printf("\n"); }

  // Por cada símbolo de la cadena
  for(int i = 0; i < num_of_symbols; i++) {
    aux_node = AF_get_transition(delta_set, states_set, string_symbols[i]);
    LSS_free(&states_set);
    states_set = aux_node;

    if(DEBUG) {
      printf("%d) Resultado = ", i);
      LSS_print(states_set);
      printf("\n");
    }
  }

  intersection = LSS_SET_new_intersection(states_set, final_set);
  if(intersection == NULL) goto error;
  bool isAccepted = LSS_LST_SET_has_data(intersection);

  if(DEBUG) { printf("final_set = "); LSS_print(final_set); printf("\n"); }
  if(DEBUG) {
    printf("Intersection = ");
    LSS_print(intersection);
    printf("\n");
  }

  sdsfreesplitres(string_symbols, num_of_symbols);
  LSS_free(&intersection);
  LSS_free(&states_set);

  return isAccepted;

  error:
  sdsfreesplitres(string_symbols, num_of_symbols);
  LSS_free(&intersection);
  LSS_free(&states_set);
  return false;
}

struct LSS_Node *AF_get_transition(
  struct LSS_Node *delta,
  struct LSS_Node *states_set,
  char *symbol
) {
  if(delta == NULL) {
    printf("<x> El conjunto de transiciones es NULL\n");
    return NULL;
  }
  if(states_set == NULL) {
    printf("<x> El estado de partida es NULL\n");
    return NULL;
  }
  if(symbol == NULL) {
    printf("<x> El simbolo a utilizar es NULL\n");
    return NULL;
  }
  if(!LSS_LST_SET_has_data(delta)) {
    printf("<x> El conjunto de transiciones no contiene elementos\n");
    return NULL;
  }

  struct LSS_Node *target_states = LSS_SET_new_empty();
  if(target_states == NULL) {
    printf("<x> Error en la asignacion de memoria dinamica\n");
    return NULL;
  }

  struct LSS_Node *temp_1 = NULL;
  struct LSS_Node *temp_2 = NULL;
  int statesSize = LSS_SET_cardinal(states_set);
  int deltaSize = LSS_SET_cardinal(delta);

  // 1) CADENA VACÍA
  if(strcmp(symbol, "") == 0) {

    DBG_log("<?> Transicion con cadena vacia\n");

    for(int i = 1; i <= statesSize; i++) {
      temp_1 = LSS_get_elem_by_pos(&states_set, i);
      temp_2 = LSS_clone(temp_1, LSS_get_elem_type(temp_1));

      int res = LSS_SET_add_element(&target_states, temp_2);

      if(res <= 0) { // -1: Error | 0: Repetido
        if(res < 0) {
          printf("<x> Error al insertar un elemento en el conjunto\n");
          LSS_free(&target_states);
          return NULL;
        }
        LSS_free(&temp_2);
      }
    }
    return target_states;
  }

  // 2) OTRAS CADENAS
  // Por cada transición de Delta
  for(int i = 1; i <= deltaSize; i++) {
    struct LSS_Node *transition = LSS_get_elem_by_pos(&delta, i);
    struct LSS_Node *fromState = LSS_get_elem_by_pos(&transition, 1);

    char *tranSym = LSS_get_elem_string(LSS_get_elem_by_pos(&transition, 2));

    // Por cada estado de entrada
    for(int j = 1; j <= statesSize; j++) {
      struct LSS_Node *state = LSS_get_elem_by_pos(&states_set, j);
      if(
        LSS_get_elem_type(fromState) == SET
        && LSS_get_elem_type(state) != SET
      ) {
        state = states_set;
      }

      if(DEBUG) {
        printf("<?> Comparacion: state = ");
        LSS_print(state);
        printf(" | fromState = ");
        LSS_print(fromState);
        printf("\n");
      }

      // Si símbolo y estado coinciden
      if(strcmp(symbol, tranSym) == 0 && LSS_compare(state, fromState) == 0) {
        struct LSS_Node *toStates = LSS_get_elem_by_pos(&transition, 3);
        int toStatesSize = LSS_SET_cardinal(toStates);

        if(DEBUG) {
          printf("<?> Transicion encontrada = ");
          LSS_print(transition);
          printf("\n");
        }

        // Por cada estado de llegada de la transición
        for(int k = 1; k <= toStatesSize; k++) {
          temp_1 = LSS_get_elem_by_pos(&toStates, k);
          temp_2 = LSS_clone(temp_1, LSS_get_elem_type(temp_1));

          int res = LSS_SET_add_element(&target_states, temp_2);

          if(res <= 0) {
            if(res < 0) {
              printf("<x> Error al insertar un elemento en el conjunto\n");
              LSS_free(&target_states);
              return NULL;
            }
            LSS_free(&temp_2);
          }
        }
      }
    }
  }
  return target_states;
}

// Crea un AFD a partir de un AFND, sin modificar el AFND.
struct LSS_Node *AF_afnd_to_afd(struct LSS_Node *afnd) {
  /*
  ** Un AF tiene la forma [Q, E, D, q0, F]
  ** El código se comentará usando el siguiente AFND. La que está entre comillas
  ** denotará cadenas (para diferenciar el conjunto {} de la cadena "{}")
  ** [{"q0","q1","q2"},{"0","1"},{["q0","0",{"q0","q1"}],...},"q0",{"q0","q2"}]
  */
  // Lista que representa al AFD
  struct LSS_Node *afd = NULL;
  // Lista de nuevos estados del AFD (luego se convierte a conjunto)
  struct LSS_Node *afd_Q = NULL;
  // Conjunto de nuevas transiciones del AFD
  struct LSS_Node *afd_D = NULL;
  // Conjunto de símbolos del AFD
  struct LSS_Node *afd_E = NULL;
  // Nuevo estado inicial del AFD
  struct LSS_Node *afd_q0 = NULL;
  // Conjunto de nuevos estados de aceptación
  struct LSS_Node *afd_F = NULL;

  // Cada estado de afd_Q representado como SET
  struct LSS_Node *afd_state_set = NULL;
  // Cada estado de afd_Q representado como STR
  struct LSS_Node *afd_state_str = NULL;
  // Cada nuevo estado que se va encontrando representado como SET
  struct LSS_Node *afd_new_state_set = NULL;
  // Cada nuevo estado que se va encontrando representado como STR
  struct LSS_Node *afd_new_state_str = NULL;
  // Cada nueva transición que se va encontrando
  struct LSS_Node *new_trans = NULL;
  // Conjunto de estados de aceptación del AFND
  struct LSS_Node *afnd_F = NULL;
  // Conjunto de transiciones del AFND
  struct LSS_Node *afnd_D = NULL;

  // Variable auxiliar que puede ser usada en 'LSS_free'
  struct LSS_Node *aux_f = NULL;
  // Variable auxiliar que NO debe ser usada en 'LSS_free'
  struct LSS_Node *aux_nf = NULL;

  // Símbolo
  char *symbol;
  // Cantidad de estados de afd_Q
  int afd_Q_size;
  // Cantidad de símbolos del AFD
  int afd_E_size;
  // Posición del estado actual en afd_Q
  int afd_Q_index = 1;

  // Conjunto de transiciones del AFND
  afnd_D = LSS_get_elem_by_pos(&afnd, D /* 3 */); // {"q0","q1","q2"}
  if(afnd_D == NULL) goto error;

  // Conjunto de estados de aceptación del AFND
  afnd_F = LSS_get_elem_by_pos(&afnd, F /* 5 */); // {"q2"}
  if(afnd_F == NULL) goto error;

  // Se inicializa la lista que contendrá al AFD
  afd = LSS_LST_new_empty(); // []
  if(afd == NULL) goto error;

  // Nuevo conjunto de símbolos (copia del original)
  aux_nf = LSS_get_elem_by_pos(&afnd, E /* 2 */);
  if(aux_nf == NULL) goto error;
  afd_E = LSS_clone(aux_nf, LSS_get_elem_type(aux_nf)); // {"0", "1"}
  if(afd_E == NULL) goto error;
  aux_nf = NULL;

  afd_E_size = LSS_SET_cardinal(afd_E);

  // Nuevo estado inicial
  afd_q0 = LSS_SET_new_empty(); // {}
  if(afd_q0 == NULL) goto error;
  aux_nf = LSS_get_elem_by_pos(&afnd, q0 /* 4 */); // "q0"
  if(aux_nf == NULL) goto error;
  if(LSS_SET_add_element(&afd_q0, aux_nf) < 0) goto error; // {"q0"}
  aux_nf = NULL;
  aux_f = LSS_clone(afd_q0, STR); // "{q0}"
  if(aux_f == NULL) goto error;
  LSS_free(&afd_q0);
  afd_q0 = aux_f;
  aux_f = NULL;

  if(DEBUG) { printf("afd_q0 = "); LSS_print(afd_q0); printf("\n"); }

  // Se inicializa la lista de nuevos estados empezando por el nuevo estado
  // inicial. Se usará una lista para evitar el ordenamiento automático y al
  // final se convertirá en un conjunto
  afd_Q = LSS_LST_new_empty(); // []
  if(afd_Q == NULL) goto error;
  aux_f = LSS_clone(afd_q0, STR);
  if(aux_f == NULL) goto error;
  if(!LSS_LST_append_element(&afd_Q, aux_f)) goto error; // ["{q0}"]
  aux_f = NULL;

  afd_Q_size = 1;

  // Se inicializa el nuevo conjunto de transiciones
  afd_D = LSS_SET_new_empty(); // {}
  if(afd_D == NULL) goto error;

  // Se inicializa el nuevo conjunto de estados de aceptación
  afd_F = LSS_SET_new_empty(); // {}
  if(afd_F == NULL) goto error;

  // Por cada estado del AFD (al principio solo está el inicial)
  while(afd_Q_index <= afd_Q_size) {
    afd_state_str = LSS_get_elem_by_pos(&afd_Q, afd_Q_index); // "{q0}" ...
    // {"q0"} ...
    afd_state_set = LSS_from_string(LSS_get_elem_string(afd_state_str));

    if(DEBUG) {
      printf("afd_state_set = ");
      LSS_print(afd_state_set);
      printf("\n");
    }

    // Por cada símbolo
    for(int j = 1; j <= afd_E_size; j++) {
      symbol = LSS_get_elem_string(LSS_get_elem_by_pos(&afd_E, j)); // "0" ...

      if(DEBUG) printf("symbol = %s\n", symbol);

      // Transición
      afd_new_state_set = AF_get_transition(afnd_D, afd_state_set, symbol);
      // {"q0","q1"} ...
      afd_new_state_str = LSS_clone(afd_new_state_set, STR); // "{q0,q1}" ...

      if(DEBUG) {
        printf("afd_new_state_str = ");
        LSS_print(afd_new_state_str); printf("\n");
      }

      // Nueva transición conseguida
      new_trans = LSS_LST_new_empty(); // []
      // ["{q0}"] ...
      LSS_LST_append_element(&new_trans, LSS_clone(afd_state_str, STR));
      // ["{q0}","0"] ...
      LSS_LST_append_element(&new_trans, LSS_from_string(symbol));

      aux_f = LSS_SET_new_empty(); // {}
      // {"{q0,q1}"} ...
      if(LSS_SET_add_element(&aux_f, LSS_clone(afd_new_state_str, STR)) < 0) {
        goto error;
      }
      // ["{q0}","0",{"{q0,q1}"}] ...
      LSS_LST_append_element(&new_trans, LSS_clone(aux_f, SET));
      LSS_free(&aux_f);

      if(DEBUG) { printf("new_trans = "); LSS_print(new_trans); printf("\n"); }

      LSS_SET_add_element(&afd_D, new_trans); // {["{q0}","0",{"{q0,q1}"}],...}

      if(DEBUG) { printf("afd_D = "); LSS_print(afd_D); printf("\n"); }

      // El nuevo estado es no vacío?
      if(LSS_LST_SET_has_data(afd_new_state_set)) {
        // El estado conseguido es realmente nuevo?
        if(LSS_get_pos_by_elem(afd_Q, afd_new_state_str) == 0) {
          // ["{q0}","{q0,q1}",...]
          LSS_LST_append_element(&afd_Q, LSS_clone(afd_new_state_str, STR));
          afd_Q_size++;
        }
      }

      if(DEBUG) { printf("afd_Q = "); LSS_print(afd_Q); printf("\n"); }

      LSS_free(&afd_new_state_set);
      LSS_free(&afd_new_state_str);
    }
    // Estados de aceptación (al principio es {"q0"}.{"q0","q2"})
    aux_f = LSS_SET_new_intersection(afd_state_set, afnd_F); // {"q0"} ...

    if(DEBUG) { printf("Interseccion = "); LSS_print(aux_f); printf("\n"); }

    if(LSS_LST_SET_has_data(aux_f)) {
      // {"{q0}",...}
      LSS_SET_add_element(&afd_F, LSS_clone(afd_state_str, STR));
    }

    if(DEBUG) {
      printf("afd_F = ");
      LSS_print(afd_F);
      printf("\n");
      printf("\n");
    }

    LSS_free(&afd_state_set);
    LSS_free(&aux_f);
    afd_Q_index++;
  }
  // Cambio de LST a SET
  aux_f = LSS_clone(afd_Q, SET); // {"{q0}","{q0,q1}",...}
  if(aux_f == NULL) goto error;
  LSS_free(&afd_Q);
  afd_Q = aux_f;

  // [{"{q0}","{q0,q1}",...}]
  LSS_LST_append_element(&afd, afd_Q);
  // [{"{q0}","{q0,q1}",...},{"0","1"}]
  LSS_LST_append_element(&afd, afd_E);
  // [{"{q0}","{q0,q1}",...},{"0","1"},{["{q0}","0",{"{q0,q1}"}],...}]
  LSS_LST_append_element(&afd, afd_D);
  // [{"{q0}","{q0,q1}",...},{"0","1"},{["{q0}","0",{"{q0,q1}"}],...},"{q0}"]
  LSS_LST_append_element(&afd, afd_q0);
  /*[
    {"{q0}","{q0,q1}",...},
    {"0","1"},
    {
      ["{q0}","0",{"{q0,q1}"}],
      ...
    },
    "{q0}",
    {"{q0}",...}
  ]*/
  LSS_LST_append_element(&afd, afd_F);
  return afd;

  error:
  LSS_free(&afd);
  LSS_free(&afd_Q);
  LSS_free(&afd_D);
  LSS_free(&afd_E);
  LSS_free(&afd_q0);
  LSS_free(&afd_F);
  LSS_free(&afd_state_set);
  LSS_free(&afd_new_state_set);
  LSS_free(&new_trans);
  LSS_free(&aux_f);
  return NULL;
}

/* Condiciones para ser AFND
 * - Si en d, un estado tiene más de una transición con el mismo símbolo
 * - Si q0 es un conjunto
 * - Si el destino de una transición tiene mas de un elemento */
enum AF_Type AF_get_type(struct LSS_Node *af) {
  if(af == NULL) {
    printf("<x> El automata finito es NULL\n");
    return NONE;
  }
  if(LSS_get_elem_type(af) != LST) {
    printf("<x> El automata finito no es valido\n");
    return NONE;
  }
  if(LSS_LST_size(af) < 5) {
    printf("<x> El automata finito no esta completo\n");
    return NONE;
  }

  struct LSS_Node *start_state = LSS_get_elem_by_pos(&af, q0);
  if(start_state == NULL) {
    printf("<x> El estado inicial es NULL\n");
    return NONE;
  }
  if(LSS_get_elem_type(start_state) != STR) return AFND;

  struct LSS_Node *delta_set = LSS_get_elem_by_pos(&af, D);
  if(start_state == NULL) {
    printf("<x> El conjunto de transiciones es NULL\n");
    return NONE;
  }
  int deltaSize = LSS_SET_cardinal(delta_set);

  char *fromStatePrev = "";
  char *symbolPrev = "";
  for(int i = 1; i <= deltaSize; i++) {

    struct LSS_Node *transition = LSS_get_elem_by_pos(&delta_set, i);

    struct LSS_Node *toStates = LSS_get_elem_by_pos(&transition, 3);

    if(LSS_get_elem_type(toStates) != SET) {
      printf("<x> El tercer elemento de una transicion debe ser un conjunto\n");
      return NONE;
    }
    if(LSS_SET_cardinal(toStates) > 1) return AFND;

    struct LSS_Node *fromState = LSS_get_elem_by_pos(&transition, 1);
    if(LSS_get_elem_type(fromState) != STR) {
      printf("<x> El primer elemento de una transicion debe ser una cadena\n");
      return NONE;
    }
    char *fromStateString = LSS_get_elem_string(fromState);
    struct LSS_Node *symbol = LSS_get_elem_by_pos(&transition, 2);
    if(LSS_get_elem_type(fromState) != STR) {
      printf("<x> El segundo elemento de una transicion debe ser una cadena\n");
      return NONE;
    }
    char *symbolString = LSS_get_elem_string(symbol);
    if(i != 1) {
      if(
        strcmp(fromStatePrev, fromStateString) == 0
        && strcmp(symbolPrev, symbolString) == 0
      ) {
        return AFND;
      }
      fromStatePrev = fromStateString;
      symbolString = symbolPrev;
    }
  }
  return AFD;
}

// CADENAS

// Ingreso de una cadena de cualquier tamaño (asignación de memoria dinámica)
char *SDS_new_user_input(void) {
  bool isAscii = true;
  size_t memoryBlock = 16;
  char * string;
  char * resultString;
  char character;
  size_t size = 0;

  string = realloc(NULL, sizeof * string * memoryBlock);
  if(!string) {
    printf("<x> Error en la asignacion dinamica de memoria\n");
    return string;
  }
  while(EOF != (character = fgetc(stdin)) && character != '\n') {
    string[size++] = character;
    if(size == memoryBlock) {
      memoryBlock *= 2;
      string = realloc(string, sizeof * string * (memoryBlock));
      if(!string) {
        printf("<x> Error en la asignacion dinamica de memoria");
        return string;
      }
    }
    if(character < 32 || character > 126) isAscii = false;
  }
  string[size++] = '\0';
  if(!isAscii) {
    printf("<!> Ha ingresado caracteres que pueden provocar errores\n");
  }

  // Convierte a cadena compatible con las funciones de sds
  resultString = sdsnew(string);
  free(string);
  return resultString;
}

void SDS_clear(char *str) {
  str[0] = '\0';
  // Actualiza el tamaño de la sds a 0
  sdsupdatelen(str);
  // Elimina la memoria extra previamente asignada
  str = sdsRemoveFreeSpace(str);
}

// INTERFAZ

int UI_new_user_option(int min, int max) {
  int x = -1;
  while(1) {
    printf("# ");
    scanf("%d", &x);
    UI_clear_stdin();
    if(x < min || x > max) {
      printf("<!> Opcion invalida\n");
    } else return x;
  }
}

void UI_pause(void) {
  printf("Presione ENTER para continuar . . .");
  char c;
  // UI_clear_stdin(); // No sé si quien llama esta función, ya limpió el buffer
  scanf("%c", &c);
  // UI_clear_stdin(); // Limpio el buffer al finalizar el llamado

}

void UI_clear_console(void) {
  system("clear || cls");
}

void UI_clear_stdin(void) {
  char c;
  while((c = getchar()) != '\n' && c != EOF);
}

int UI_main_menu(enum AF_Type af_type) {
  int optionMin = 0;
  int optionMax = 1;
  if(af_type == NONE) { // Primera iteración
    printf("<i> No hay un automata finito cargado\n");
    printf("1. Cargar automata finito\n");
  } else { // Resto de iteraciones
    if(af_type == AFD) {
      printf("<i> El automata finito cargado es determinista\n");
    } else {
      printf("<i> El automata finito cargado es no determinista\n");
    }
    printf("1. Volver a cargar automata finito\n");
    printf("2. Mostrar el automata finito cargado\n");
    printf("3. Generar grafico de la estructura cargada\n");
    printf("4. Probar la aceptacion de una cadena en el automata finito\n");
    // Si el AF es un AFND
    if(af_type == AFND) {
      printf("5. Convertir automata finito no determinista en determinista\n");
      optionMax = 5;
    } else optionMax = 4;
  }
  // Se muestra siempre
  printf("0. Salir\n");
  return UI_new_user_option(optionMin, optionMax);
}

struct LSS_Node *UI_new_af(void) {
  struct LSS_Node *af = NULL;
  char *userString;

  printf("Carga de un automata finito\n");
  printf("Metodo de carga\n");
  printf("1. Por partes\n"); // Falta implementar la validación
  printf("2. Directa\n"); // 

  switch(UI_new_user_option(1, 2)) {
    case 1:
      printf("Carga por partes\n");
      af = LSS_LST_new_empty();

      printf("Ingrese el conjunto de estados\n# ");
      userString = SDS_new_user_input();
      // Verificar que sea un conjunto de estados válido
      if(!LSS_LST_append_element(&af, LSS_from_string(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de simbolos del alfabeto\n# ");
      userString = SDS_new_user_input();
      // Verificar que sea un conjunto de símbolos válido
      if(!LSS_LST_append_element(&af, LSS_from_string(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de transiciones\n# ");
      userString = SDS_new_user_input();
      // Verificar que sea un conjunto de transiciones válido
      if(!LSS_LST_append_element(&af, LSS_from_string(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el estado inicial\n# ");
      userString = SDS_new_user_input();
      // Verificar que sea un estado inicial válido
      if(!LSS_LST_append_element(&af, LSS_from_string(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);

      printf("Ingrese el conjunto de estados de aceptacion\n# ");
      userString = SDS_new_user_input();
      // Verificar que sea un conjunto de estados de aceptación válido
      if(!LSS_LST_append_element(&af, LSS_from_string(userString))) {
        printf("<x> Error al insertar el conjunto de estados");
        goto error;
      }
      sdsfree(userString);
      break;

    case 2:
      printf("Carga directa\n");
      printf("Ingrese el automata finito completo\n");
      printf("# ");
      userString = SDS_new_user_input();
      af = LSS_from_string(userString);
      sdsfree(userString);
      break;
  }
  return af;

  error:
  LSS_free(&af);
  sdsfree(userString);
  return NULL;
}

void UI_print_af(struct LSS_Node *af) {
  printf("Estados\n");
  LSS_print(LSS_get_elem_by_pos(&af, Q));

  printf("\nSimbolos de entrada\n");
  LSS_print(LSS_get_elem_by_pos(&af, E));

  printf("\nTransiciones\n");
  struct LSS_Node *temp = LSS_get_elem_by_pos(&af, D);
  int max = LSS_SET_cardinal(temp);
  for(int i = 1; i <= max; i++) {
    LSS_print(LSS_get_elem_by_pos(&temp, i));
    printf("\n");
  }

  printf("Estado inicial\n");
  LSS_print(LSS_get_elem_by_pos(&af, q0));
  printf("\nEstados de aceptacion\n");
  LSS_print(LSS_get_elem_by_pos(&af, F));
  printf("\n");
}

void UI_acceptance_menu(struct LSS_Node *automata_finito) {
  char *userString;
  printf("Ingrese la cadena a evaluar\n# ");
  userString = SDS_new_user_input();
  if(AF_is_string_accepted(automata_finito, userString)) {
    printf(
      "La cadena \"%s\" SI ES ACEPTADA por el automata finito\n",
      userString
    );
  } else printf(
    "La cadena \"%s\" NO ES ACEPTADA por el automata finito\n",
    userString
  );
  sdsfree(userString);
}

void UI_graphviz_menu(struct LSS_Node *automata_finito) {
  printf("Los archivos se crearan en la carpeta donde esta este programa\n");
  printf("Ingrese un nombre para los archivos\n# ");
  char *name = SDS_new_user_input();
  printf("Ingrese el formato de salida (png, jpg, svg, etc.)\n# ");
  char *format = SDS_new_user_input();
  printf("Ingrese la resolucion en PPP/DPI (recomendado: 300)\n");
  int dpi = UI_new_user_option(1, 1000);

#ifdef _WIN32
  char *no_echo = "1>nul 2>nul";
  char *start_viewer = sdscatfmt(
    sdsempty(),
    "start %S.%S %s",
    name,
    format,
    no_echo
  );
#else
  char *no_echo = "1>/dev/null 2>/dev/null";
  char *start_viewer = sdscatfmt(
    sdsempty(),
    "open %S.%S %s",
    name,
    format,
    no_echo
  );
#endif

  char *cmd = sdscatfmt(
    sdsempty(),
    "dot -T%S -Gdpi=%i %S.dot -o %S.%S %s",
    format,
    dpi,
    name,
    name,
    format,
    no_echo
  );
  LSS_gen_dotfile(automata_finito, name);
  printf("Generando archivo %s.%s\n", name, format);
  int r = system(cmd);
  DBG_log("<?> El comando \"%s\" retorna: %d\n", cmd, r);
  switch(r) {
    case 0:
      printf("Imagen generada. Abriendo...\n");
      r = system(start_viewer);
      DBG_log("<?> El comando \"%s\" retorna: %d\n", start_viewer, r);
      if(!r) break;
      printf("<x> Algo salio mal\n");
      break;
    case 1: case 127: case 9009: case 32512:
      printf(
        "Graphviz no encontrado. Para generar el grafico, instale Graphviz: "
        "https://graphviz.org/download\n"
      );
      break;
    default:
      printf("<x> Error al generar la imagen\n");
  }
  sdsfree(start_viewer);
  sdsfree(name);
  sdsfree(format);
  sdsfree(cmd);
}
