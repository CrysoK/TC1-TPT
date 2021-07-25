# Changelog

## Unreleased

- Convertir **AFND** en **AFD**.
- Validación de un **AF** mientras el usuario lo va ingresando.
- Visualización de la estructura de datos utilizada.

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

- `compareStrings` (nuevo)
- `cloneLSSNode` (nuevo)
- `compareStrings` (nuevo)
- `getDataByPos` (nuevo)
- `getNodeType` (nuevo)
- `compareNodes` (nuevo)
- `addStringToSet` (nuevo)
- `hasDataLS` (nuevo)
- `getTransition` (nuevo)
- `isStringAccepted` (nuevo)
- `splitStringBySymbols` (nuevo)
- `getAfType` (nuevo)
- `mainMenu` (nuevo)
- `newAf` (nuevo)
- `printAf` (nuevo)
- `acceptanceMenu` (nuevo)
- `getStringFromNode`
- `newEmptySet`
- `newIntersectionSet`

#### Modificados

- `main`
- `getDataPriority`
- `compareNodes`
- `freeLSS`
- `errorMessages`
- `getElementByPos`
- `compareLSS`
- `appendStringToList`
- `isStringAListOrSet`
- `getSetCardinal`
- `getListSize`

#### Incompletos

- `isStringElementOfLS`
- `isElementOfList`
- `isElementOfSet`
- `areSetsEqual`
- `newPowerSet`
- `newUnionSet`

#### Eliminados

- `isEmptyLS`
- `isAfnd`

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

- `getElementByPos` (nuevo)
- `getListSize` (nuevo)
- `getSizeOfLS` (nuevo)
- `addNodeInOrder`

#### Modificados

- `main`
- `newLSFromString`
- `newLSFromStringRecursive`
- `printLS`
- `newStringNode`
- `getSetCardinal`
- `newEmptyList`
- `isStringAListOrSet`
- `getStringType`
- `splitString`
- `isStringANumber`
- `getDataPriority`

#### Incompletos

- `isAfnd` (nuevo)
- `isStringElementOfLS`
- `getStringFromNode`
- `isElementOfList`
- `isElementOfSet`
- `areSetsEqual`
- `newPowerSet`
- `newIntersectionSet`
- `newUnionSet`
- `newEmptySet`

#### Eliminados

- `isStringValid`

## [0.5.0](https://github.com/CrysoK/TCI_TPT/releases/tag/0.5.0) - 2021-06-01

- Versión del trabajo práctico transversal usada en la primera entrega.
