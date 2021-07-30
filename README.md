![GitHub release (latest SemVer including pre-releases)](https://img.shields.io/github/v/release/crysok/tci_tpt?include_prereleases&style=flat-square)

# Trabajo práctico transversal

## Descripción

> ¿Mucho texto? Avanza hasta [instalación](#instalaci%C3%B3n) o [cómo usar](#c%C3%B3mo-usar).

El trabajo está compuesto por dos partes.

La primera es un TAD (tipo abstracto de datos) que permite modelar los objetos matemáticos conjuntos y listas, cuyos componentes más elementales son cadenas de caracteres pero también pueden contener conjuntos o listas de forma recursiva, con cualquier nivel de anidamiento o profundidad.

La segunda utiliza el TAD diseñado para implementar algoritmos de autómatas finitos.

### TAD

**L**ST (listas), **S**ET (conjuntos) y **S**TR (cadenas) son los tres tipos de datos que puede representar un nodo **LSS**.

```c
enum NODE_TYPE { LST, SET, STR };

struct LSSNode {
  enum NODE_TYPE type;
  union {
    char *str;
    struct {
      struct LSSNode *data;
      struct LSSNode *next;
    };
  };
};
```

#### Conjuntos

- Se simbolizan con una llave de apertura, una secuencia de elementos separados por comas y una llave de cierre. Ejemplo: `{uno, dos, tres}`.
- No poseen elementos repetidos.
- No tienen orden. El conjunto `{uno, dos, tres}` es el mismo conjunto que `{dos, uno, tres}` o que `{tres, dos, uno}`. En el programa se ordenan para simplificar operaciones como la comparación.
- Aritmética de conjuntos:
  - Unión (no implementado).
  - Intersección.
  - Diferencia (no implementado).
  - Pertenencia.
  - Cardinal.

#### Listas

- Se simbolizan con un corchete de apertura, la lista de elementos separados por comas y un corchete de cierre. Ejemplo: `[uno, dos, tres]`.
- Pueden tener elementos repetidos.
- Tienen orden. La lista `[uno, dos, tres]` es distinta que `[dos, uno, tres]` o que `[tres, dos, uno]`.
- Aritmética de listas:
  - Agregar elementos al inicio y al final.
  - Remover elementos al inicio y al final (no implementado).
  - Pertenencia.
  - Tamaño de la lista.

### Autómatas finitos

Los autómatas finitos pueden ser deterministas o no deterministas. En ambos casos su estructura es una *5-upla* definida por ( 𝑄 , 𝛴 , 𝛿 , 𝑞₀ , 𝐹 ) en donde:

- 𝑄 es el conjunto de estados,
- 𝛴 es el conjunto de símbolos de entrada,
- 𝑞₀ es el estado inicial,
- 𝐹 el conjunto de estados de aceptación
- 𝛿, cuando el AF es determinista, es una función de 𝑄 × 𝛴 en 𝑄 y, cuando el AF es no determinista, es una relación en 𝑄 × 𝛴 × 𝑄. Se puede pensar a 𝛿 como una terna o lista de tres elementos que representan el estado de partida, el carácter leído, y los estados destino (puede ser uno) respectivamente.

La transición de un AFD 𝛿 { 𝑞₀ , a } = 𝑞₁ se puede representar como [ 𝑞₀ , a , 𝑞₁ ].

La transición de un AFND 𝛿 { 𝑞₀ , a } = { 𝑞₁ , 𝑞₂ } se puede representar como  [𝑞₀ , a , { 𝑞₁ , 𝑞₂ } ].

Como los AFD son casos particulares de un AFND, se puede pensar en almacenar a los AFD con su tercera componente como un conjunto de un solo elemento. Así, generalizando, se representa a 𝛿 como una terna o lista de tres elementos cuyo primer elemento es una cadena (el estado), segundo elemento también una cadena (el símbolo leído) y tercer elemento un conjunto de cadenas (conjunto de estados destino).

Entonces un AF puede ser almacenado con la siguiente estructura:

[ { 𝑞₀ , 𝑞₁ , ... , 𝑞ₙ } , { a , b } , { [ 𝑞₀ , a , { 𝑞₀ , 𝑞₁ } ] , ... , [ 𝑞₂ , b , { 𝑞₁ } ] } , 𝑞₀ , { 𝑞₂ } ]

Donde:

- 𝑄 = { 𝑞₀ , 𝑞₁ , ... , 𝑞ₙ }
- 𝛴 = { a , b }
- 𝛿 = { [ 𝑞₀ , a , { 𝑞₀ , 𝑞₁ } ] , ... , [ 𝑞₂ , b , { 𝑞₁ } ] }
- 𝑞₀ = 𝑞₀
- 𝐹 = { 𝑞₂ }

Se puede concluir que el TAD diseñado permite almacenar autómatas finitos.

### Objetivos

Implementar el [TAD](#tad) descripto utilizando la estructura de datos propuesta. Luego utilizarlo en un programa para las siguientes tareas:

- Cargar un AF "por partes": primero el conjunto de estados, luego el alfabeto, etc. (cargar la transición de estados por terna).
- Almacenar el AF en la estructura propuesta.
- Mostrar el autómata finito.
- Dada una cadena, indicar si pertenece al lenguaje regular cuyo autómata finito es el ingresado.
- Si el AF es no determinista, convertirlo en determinista.

## Instalación

### Windows

Puede descargar el ejecutable de la última versión [aquí](https://github.com/CrysoK/TCI_TPT/releases).

También puede compilar el programa por su cuenta. Lo más simple es usar `MakeWindows.cmd`. [Más información](https://github.com/CrysoK/C-Makefile).

### Linux

Debe compilar el programa por su cuenta. Lo más simple es usar `MakeLinux.sh`. [Más información](https://github.com/CrysoK/C-Makefile).

## ¿Cómo usar?

El programa cuenta con una interfaz sencilla basada en menús y opciones que se indican por teclado.

Algunos mensajes utilizan ciertos símbolos, que indican lo siguiente:

- `<x>` Mensaje de error.`
- `<!>` Mensaje de advertencia.
- `<i>` Información secundaria.
- `<?>` Mensajes que se activan al usar `#DEBUG 1` en el código fuente.
- `#`  Se espera que el usuario ingrese datos.

### Carga de un autómata finito

Puede cargar cualquier AF siempre que sea correcto y se respete el formato indicado más [arriba](#autómatas-finitos).

[Estos](afs.md) AF pueden servirle para probar el programa. La forma más rápida de cargarlos es copiar y pegar en la opción de carga directa.

### Visualización de un autómata finito

#### Consola

Basta con elegir la opción 2 para ver al AF cargado como texto simple en la consola.

#### Externa

La opción 3 genera el archivo `LSS.dot` en la misma carpeta en la que se encuentra el ejecutable. Este archivo se puede usar con [Graphviz](https://graphviz.org/) para crear una representación gráfica mucho más detallada de los datos cargados.

Está en los planes que el programa genere directamente una imagen si el usuario tiene Graphviz instalado.

### Aceptación de cadenas

La opción 4 permite ingresar cadenas y comprobar si el AF cargado las acepta o no. Las cadenas deben estar formadas por símbolos que estén definidos en el AF.

### Conversión

En el caso de cargar un AFND, se muestra la opción 5 para convertirlo en un AFD.
El AFD resultante es completamente válido y se puede usar en el resto de opciones.
