# Changelog

## [0.9.0](https://github.com/CrysoK/TCI_TPT/releases/tag/0.9.0) - 2022-03-19

- Ahora la opción 3 permite generar una imagen y mostrarla cuando
  [Graphviz](https://graphviz.org/download/) está instalado.
- Correcciones/mejoras menores.

## [0.8.0](https://github.com/CrysoK/TCI_TPT/releases/tag/0.8.0) - 2021-07-30

### General

- Conversión de **AFND** en **AFD**.
- Generación del archivo `LSS.dot` que puede ser interpretado por [Graphviz](https://graphviz.org/) para visualizar el **AF** cargado.

### Módulos

#### Terminados

- `appendElementToList` (nuevo)
- `addElementToSet` (nuevo)
- `generateDotFile` (nuevo)
- `stringFromLss` (nuevo)
- `genDotNodes` (nuevo)
- `afndToAfd` (nuevo)

#### Modificados

- `addNodeInOrder`
- `cloneLss` (antes `cloneLSSNode`)
- `compareLss` (antes `compareLSS`)
- `freeLss` (antes `freeLSS`)
- `getAfType`
- `getElementString` (antes `getStringFromNode`)
- `getElementType` (antes `getNodeType`)
- `getTransition`
- `hasDataLs`
- `hasDataLs` (antes `hasDataLS`)
- `isStringAccepted`
- `isStringAListOrSet`
- `main`
- `mainMenu`
- `newAf`
- `newIntersectionSet`
- `newLsFromStringRecursive`
- `newLssFromString` (antes `newLSFromString`)
- `printAf`
- `printLsRecursive`
- `printLss` (antes `printLS`)

#### Incompletos

- `newPowerSet`
- `newUnionSet`

#### Eliminados

- `addStringToSet`
- `appendStringToList`
- `areSetsEqual`
- `deleteStringSpaces`
- `getDataByPos`
- `isElementOfList`
- `isElementOfSet`

## [0.7.0](https://github.com/CrysoK/TCI_TPT/releases/tag/0.7.0) - 2021-07-25

### General

- Función para verificar si una cadena es aceptada (o no) por un autómata finito.
- Ahora se indica si lo ingresado es un **AFND**, **AFD** o ninguno.
- Librería `natsort` añadida para ordenar cadenas de forma "natural".
- Librería `auxlib` eliminada por ya no ser necesaria.
- Ordenamiento mejorado.
- Manejo de errores mejorado.

### Módulos

#### Terminados

- `acceptanceMenu` (nuevo)
- `addStringToSet` (nuevo)
- `cloneLSSNode` (nuevo)
- `compareNodes` (nuevo)
- `compareStrings` (nuevo)
- `compareStrings` (nuevo)
- `getAfType` (nuevo)
- `getDataByPos` (nuevo)
- `getNodeType` (nuevo)
- `getStringFromNode`
- `getTransition` (nuevo)
- `hasDataLS` (nuevo)
- `isStringAccepted` (nuevo)
- `mainMenu` (nuevo)
- `newAf` (nuevo)
- `newEmptySet`
- `newIntersectionSet`
- `printAf` (nuevo)
- `splitStringBySymbols` (nuevo)

#### Modificados

- `appendStringToList`
- `compareLSS`
- `compareNodes`
- `errorMessages`
- `freeLSS`
- `getDataPriority`
- `getElementByPos`
- `getListSize`
- `getSetCardinal`
- `isStringAListOrSet`
- `main`

#### Incompletos

- `areSetsEqual`
- `isElementOfList`
- `isElementOfSet`
- `isStringElementOfLS`
- `newPowerSet`
- `newUnionSet`

#### Eliminados

- `isAfnd`
- `isEmptyLS`

## [0.6.1](https://github.com/CrysoK/TCI_TPT/releases/tag/0.6.1) - 2021-07-13

- CHANGELOG.md añadido.
- README.md modificado.
- Archivos `.d` eliminados.

## [0.6.0](https://github.com/CrysoK/TCI_TPT/releases/tag/0.6.0) - 2021-07-13

### General

- Implementación de un menú.
- Inserción ordenada en conjuntos.
- Conjuntos sin elementos repetidos.
- Eliminación de espacios alrededor de las cadenas al convertirlas en nodos.
- Llamados a `sdslen` reemplazados por `strlen`.

### Módulos

#### Terminados

- `addNodeInOrder`
- `getElementByPos` (nuevo)
- `getListSize` (nuevo)
- `getSizeOfLS` (nuevo)

#### Modificados

- `getDataPriority`
- `getSetCardinal`
- `getStringType`
- `isStringAListOrSet`
- `isStringANumber`
- `main`
- `newEmptyList`
- `newLSFromString`
- `newLSFromStringRecursive`
- `newStringNode`
- `printLS`
- `splitString`

#### Incompletos

- `areSetsEqual`
- `getStringFromNode`
- `isAfnd` (nuevo)
- `isElementOfList`
- `isElementOfSet`
- `isStringElementOfLS`
- `newEmptySet`
- `newIntersectionSet`
- `newPowerSet`
- `newUnionSet`

#### Eliminados

- `isStringValid`

## [0.5.0](https://github.com/CrysoK/TCI_TPT/releases/tag/0.5.0) - 2021-06-01

- Versión del trabajo práctico transversal usada en la primera entrega.
