# Trabajo práctico transversal

## Información

### [Progreso](https://crysok.notion.site/ae7b8a639c6b4d9fba998113b64b34c5?v=1f1ea6e9819e4484a3326136c6f5bf11)

### [Registro de cambios](CHANGELOG.md)

## Ejemplos de AF

### AFD

#### Cadenas que comienzan con `ab`

```plaintext
[{q0,q1,q2},{a,b},{[q0,a,{q1}],[q0,b,{}],[q1,a,{}],[q1,b,{q2}],[q2,a,{q2}],[q2,b,{q2}]},q0,{q2}]
```

#### Cadenas que terminan con `ab`

```plaintext
[{q0,q1,q2},{a,b},{[q0,a,{q1}],[q0,b,{q0}],[q1,a,{q1}],[q1,b,{q2}],[q2,a,{q1}],[q2,b,{q0}]},q0,{q2}]
```

#### Cadenas que contienen `011`

```plaintext
[{q0,q1,q2,q3},{0,1},{[q0,0,{q1}],[q0,1,{q0}],[q1,0,{q1}],[q1,1,{q2}],[q2,0,{q1}],[q2,1,{q3}],[q3,0,{q3}],[q3,1,{q3}]},q0,{q3}]
```

#### Cadenas que representan números binarios divisibles por 3

```plaintext
[{r0,r1,r2},{0,1},{[r0,0,{r0}],[r0,1,{r1}],[r1,0,{r2}],[r1,1,{r0}],[r2,0,{r1}],[r2,1,{r2}]},r0,{r0}]
```

#### Cadenas que representan números binarios **no** divisibles por 3

```text
[{r0,r1,r2},{0,1},{[r0,0,{r0}],[r0,1,{r1}],[r1,0,{r2}],[r1,1,{r0}],[r2,0,{r1}],[r2,1,{r2}]},r0,{r1,r2}]
```

#### Cadenas aceptadas por la expresión regular `0(00)*0+ε`

```text
[{q0,q1,q2,q3},{0,1},{[q0,0,{q1}],[q0,1,{q3}],[q1,0,{q2}],[q1,1,{q2}],[q2,0,{q1}],[q2,1,{q3}],[q3,0,{q3}],[q3,1,{q3}]},q0,{q0,q2}]
```

#### Cadenas que contienen `01`

```text
[{q0,q1,q2},{0,1},{[q0,0,{q2}],[q0,1,{q0}],[q2,0,{q2}],[q2,1,{q1}],[q1,0,{q1}],[q1,1,{q1}]},q0,{q1}]
```

#### Sin descripción 1

```text
[{q0,q1,q2,q3},{0,1},{[q0,0,{q2}],[q0,1,{q1}],[q1,0,{q3}],[q1,1,{q0}],[q2,0,{q0}],[q2,1,{q3}],[q3,0,{q1}],[q3,1,{q2}]},q0,{q0}]
```

Acepta: cadena vacía, `00110101`

No acepta: `111`, `110111`, `1000`, `01`

#### Cadenas que terminan en "00"

```text
[{I,0,00},{0,1},{[I,1,{I}],[I,0,{0}],[0,1,{I}],[0,0,{00}],[00,1,{I}],[00,0,{00}]},I,{00}]
```

#### Cadenas que contienen al menos un `1`

```text
[{I,1},{0,1},{[I,0,{I}],[I,1,{1}],[1,0,{1}],[1,1,{1}]},I,{1}]
```

#### Cadenas que contienen `000` en cualquier posición

```text
[{I,0,00,000},{0,1},{[I,0,{0}],[I,1,{I}],[0,0,{00}],[0,1,{I}],[00,0,{000}],[00,1,{I}],[000,0,{000}],[000,1,{000}]},I,{000}]
```

#### Cadenas formadas por `n` veces `a` y `m` veces `b` tal que: `(n + m) MOD 3 = 1`

```text
[{q0,q1,q2,q3,q4,q5,q6},{a,b},{[q0,a,{q1}],[q0,b,{q4}],[q1,a,{q2}],[q1,b,{q5}],[q2,a,{q3}],[q2,b,{q6}],[q3,a,{q1}],[q3,b,{q4}],[q4,b,{q5}],[q5,b,{q6}],[q6,b,{q4}]},q0,{q1,q4}]
```

[Este](#lenguaje-invertido-de-este-autómata) autómata acepta el mismo lenguaje pero invertido.

### AFND

#### Sin descripción 2

```text
[{q0,q1,q2,q3,q4,q5},{0,1},{[q0,0,{q0}],[q0,1,{q0,q1,q2}],[q1,0,{q2,q3}],[q1,1,{}],[q2,0,{}],[q2,1,{q4}],[q3,0,{q5}],[q3,1,{}],[q4,0,{}],[q4,1,{q5}],[q5,0,{q5}],[q5,1,{q5}]},{q0},{q5}]
```

#### Sin descripción 3

```text
[{p0,p1,p2},{a,b},{[p0,a,{}],[p0,b,{p2}],[p1,a,{p0}],[p1,b,{p1,p2}],[p2,a,{p0,p1,p2}],[p2,b,{}]},{p0},{p1}]
```

#### Lenguaje invertido de [este](#cadenas-formadas-por-n-veces-a-y-m-veces-b-tal-que-n-m-mod-3-1`) autómata

```text
[{s0,q0,q1,q2,q3,q4,q5,q6},{a,b},{[q1,a,{q0,q3}],[q2,a,{q1}],[q3,a,{q2}],[q4,b,{q0,q3,q6}],[q5,b,{q4,q1}],[q6,b,{q2,q5}],[s0,a,{q0,q3}],[s0,b,{q3,q0,q6}]},{s0},{q0}]
```
