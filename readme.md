# Tarea 2

## Laboratorio

Se presentan los siguientes ejercicios:

### Hola Mundo

```bash
git checkout tags/holamundo
make
make run
```

### Hola Mundo 2

```bash
git checkout tags/holamundo2
make
make run
```

### Comunicacion punto a punto

```bash
git checkout tags/lab-puntoapunto1
make
make run

git checkout tags/lab-puntoapunto2
make
make run

git checkout tags/lab-puntoapunto3
make
make run
```

### Comunicacion colectiva

```bash
git checkout tags/lab-comunicacioncolectiva1
make
make run

git checkout tags/lab-comunicacioncolectiva2
make
make run
```

### Producto vector 8x1 matriz 8x8

```bash
git checkout tags/lab-productovectormatriz
make
make run
```

## Ejercicio 1

### Producto vector matriz secuencial

Se presenta la solución utilizando únicamente 1 de los ranks existentes.

```bash
git checkout tags/ej1-secuencial
make
make run
```

### Producto vector matriz paralelo

El algoritmo presentado requiere al menos 2 ranks, la tarea se paraleliza en n-1 ranks, n siendo el total de ranks.

```bash
git checkout tags/ej1-paralelo
make
make run
```

## Ejercicio 2

### Ruta más cercana secuencial

Se obtiene la ruta circular más corta según los datos en el enunciado.

```bash
git checkout tags/ej2-secuencial
make
make run
```

## Ejercicio 3

### 500000 primeros primos secuencial

Se presenta la solución llamando con solo un rank.

Se imprime el tiempo requerido para encontrar los primos.

```bash
git checkout tags/ej3-secuencial
make
make run
```

#### Para validar los resultados en secuencial con un archivo csv

Se generan en el archivo output.csv las columnas de conteo(a), resultado obtenido(b) y resultado obtenido de internet(c), se puede realizar la operación abs(b-c), luego sumar todos estos resultados, lo cual debería ser 0.

```bash
git checkout tags/ej3-validar-secuencial
./compare-primes.sh
```

### 500000 primeros primos paralelo

El algoritmo presentado requiere al menos 2 ranks, la tarea se paraleliza en n-1 ranks, n siendo el total de ranks.

Se encuentran más de 500000 primos debido al metodo de distribución de los números entre los ranks.

Se imprime el tiempo requerido.

```bash
git checkout tags/ej3-paralelo
make
make run
```

#### Para validar los resultados  en paralelo con un archivo csv

igual que en el caso secuancial, excepto que no se cuenta con una columna de conteo.

```bash
git checkout tags/ej3-validar-secuencial
./compare-primes.sh
```
