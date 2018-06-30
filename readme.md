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
