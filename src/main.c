// INFORAMCIÓN /////////////////////////////////////////////////////////////////

/*\
 * TRABAJO PRÁCTICO TRANSVERSAL - TEORÍA DE LA COMPUTACIÓN I
 *
 * PARTE 1
 * 1) Cargar un autómata finito "por partes": primero el conjunto de estados,
 * luego el alfabeto y así (la transición de estados cargarla por terna).
 * 2) Crear un árbol de que almacene al autómata finito ingresado.
 * 3) Mostrar el autómata finito.
 *
 * REFERENCIAS
 * <x> Error
 * <!> Advertencia
 * <i> Información
 * <?> Debug
 *  #  Input
 *
 * FUENTES
 * - Casteo al usar malloc: https://stackoverflow.com/a/605858
 * - SDS Simple Dynamic Strings: https://github.com/antirez/sds
 * -
 *
\*/

// PRECOMPILADOR ///////////////////////////////////////////////////////////////

#include <stdio.h> // Entrada y salida estándar
#include <stdlib.h> // Librería general estándar
#include <string.h> // Manejo de cadenas estándar
#include <stdbool.h> // Tipo de dato booleano estándar

#include "include/auxlib.h" // Declaraciones y definiciones útiles
#include "include/set-lst.h" // TAD Conjuntos y Listas
#include "include/sds/sds.h" // Librería externa "Simple Dynamic Strings"

// DECLARACIONES ///////////////////////////////////////////////////////////////

// Cadenas

// Recibe la entrada desde consola y se almacena dinámicamente
char *sds_scan();
// Elimina el contenido de una cadena sin liberar la memoria asignada
void sds_clear(char * str);
void del_line(int);

// Interfaz

int opcion(int, int);

// MAIN ////////////////////////////////////////////////////////////////////////

int main() {
  struct sl_nodo *af;
  char * entrada;
  int opc;

  printf("<i> Para desactivar mensajes \"<?>\" defina DEBUG 0 <include/auxlib.h>\n");
  printf("Carga de un automata finito\n");
  printf("Metodo de carga\n");
  printf("1. Por partes\n");
  printf("2. Todo de una vez (test)\n");

  opc = opcion(1, 2);

  switch(opc) {
    case 1:
      printf("Carga por partes\n");
      af = lst_vacia();
      
      printf("Ingrese el conjunto de estados\n");
      entrada = sds_scan();
      lst_ins_final(&af, entrada);
      sdsfree(entrada);

      printf("Ingrese el conjunto de simbolos del alfabeto\n");
      entrada = sds_scan();
      lst_ins_final(&af, entrada);
      sdsfree(entrada);
      
      printf("Ingrese el conjunto de transiciones\n");
      entrada = sds_scan();
      lst_ins_final(&af, entrada);
      sdsfree(entrada);

      printf("Ingrese el estado inicial\n");
      entrada = sds_scan();
      lst_ins_final(&af, entrada);
      sdsfree(entrada);

      printf("Ingrese el conjunto de estados de aceptacion\n");
      entrada = sds_scan();
      lst_ins_final(&af, entrada);
      sdsfree(entrada);
      break;
    case 2:
      
      printf("Carga directa\n");
      printf("Ingrese el automata finito completo\n");
      printf("# ");
      entrada = sds_scan();
      af = sl_nuevo(entrada);
      //printf("%s\n", entrada);
      sdsfree(entrada);
      break;
  }

  sl_mostrar(af);

  /*
  printf("Ingrese el conjunto o cadena:\n");
  char *entrada = sdsnew(sds_scan());
  printf("Entrada: %s\n", entrada);
  printf("\nAlmacenando entrada en la estructura... (%s:%d)", __FILE__, __LINE__);

  af = sl_nuevo(entrada);

  printf("\n\n");
  sl_mostrar(af);
  printf("\nCardinal = %d", set_cardinal(af));
  printf("\n\n");
  */

  pausa();

  return 0;
}

// DEFINICIONES ////////////////////////////////////////////////////////////////

// CADENAS

// Ingreso de una cadena de cualquier tamaño (asignación de memoria dinámica)
char *sds_scan() {
  bool no_ascii = false;
  size_t bloque = 16;
  char * str;
  char * ret;
  char caracter;
  size_t tam = 0;

  str = realloc(NULL, sizeof * str * bloque);
  if(!str) return str; // ERROR
  while(EOF != (caracter = fgetc(stdin)) && caracter != '\n') {
    str[tam++] = caracter;
    if(tam == bloque) {
      bloque *= 2;
      str = realloc(str, sizeof * str * (bloque));
      if(!str) return str; // ERROR
    }
    if(caracter < 33 || caracter > 126) no_ascii = true;
  }
  str[tam++] = '\0';
  if(no_ascii) printf("<!> Ha ingresado caracteres que pueden provocar errores\n");

  ret = sdsnew(str);
  free(str);
  return ret;
}

/* Elimina la linea pero solo funciona si no se imprimió "\n" antes */
void del_line(int n) {
  printf("\r");
  for(int i = 1; i <= n; i++) {
    printf(" ");
  }
}

void sds_clean(char *str) {
  str[0] = '\0';
  // Actualiza el tamaño de la sds a 0
  sdsupdatelen(str);
  // Elimina la memoria extra previamente asignada
  str = sdsRemoveFreeSpace(str);
}

// INTERFAZ

int opcion(int min, int max) {
  int x = -1;
  while(1) {
    printf("# ");
    scanf("%d", &x);
    cl_stdin();
    if(x < min || x > max) {
      printf("<!> Opcion invalida\n");
    } else return x;
  }
}