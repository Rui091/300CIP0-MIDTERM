# Proyecto de Procesamiento de Imágenes Paralelo

Este proyecto implementa un framework orientado a objetos en C++ para el procesamiento de imágenes en formato PGM y PPM, aplicando filtros mediante diferentes paradigmas de paralelización. El objetivo es comparar el rendimiento entre la versión secuencial y las versiones paralelas usando Pthreads, OpenMP y MPI.

## Descripción General
- **Lenguaje:** C++
- **Formatos soportados:** PGM (grises), PPM (color)
- **Filtros:** Varios (ejemplo: blur, edge detection, etc.)
- **Paralelización:**
   - Secuencial (referencia)
   - Pthreads (4 hilos)
   - OpenMP (multi-core)
   - MPI (multi-proceso)

## Estructura de Archivos Base
- `Image.h` / `Image.cpp`: Clase base para imágenes.
- `PGMImage.h` / `PGMImage.cpp`: Implementación para imágenes PGM.
- `PPMImage.h` / `PPMImage.cpp`: Implementación para imágenes PPM.
- `ImageFactory.h` / `ImageFactory.cpp`: Fábrica de imágenes.
- `Filter.h` / `Filter.cpp`: Clase base para filtros (si aplica).

## Descripción de Versiones
- **Secuencial:**
   - Archivo principal: `filterer.cpp`
   - Procesa la imagen de principio a fin en un solo hilo.
- **Pthreads:**
   - Archivo principal: `pth_filterer.cpp`
   - Divide la imagen en regiones y procesa cada una en un hilo (4 hilos).
- **OpenMP:**
   - Archivo principal: `omp_filterer.cpp`
   - Utiliza directivas OpenMP para paralelizar el procesamiento en todos los núcleos disponibles.
- **MPI:**
   - Archivo principal: `mpi_filterer.cpp`
   - Distribuye filas de la imagen entre procesos MPI, con comunicación maestro-trabajador.

## Compilación
Utiliza el `Makefile` incluido para compilar todas las versiones:

```sh
make all
```

Esto generará los ejecutables para cada versión.

## Ejecución de Cada Versión

### 1. Secuencial
```sh
./filterer <input.pgm/ppm> <output.pgm/ppm> <filtro>
```

### 2. Pthreads
```sh
./pth_filterer <input.pgm/ppm> <output.pgm/ppm> <filtro>
```

### 3. OpenMP
```sh
./omp_filterer <input.pgm/ppm> <output.pgm/ppm> <filtro>
```

### 4. MPI
#### a) En una sola máquina (local):
```sh
mpirun -np <N> ./mpi_filterer <input.pgm/ppm> <output.pgm/ppm> <filtro>
```
Donde `<N>` es el número de procesos MPI (usualmente igual al número de núcleos o mayor).

#### b) En Docker Compose (multi-contenedor):
1. Levanta el clúster:
    ```sh
    docker compose -f docker-compose-mpi-workers.yml up -d
    ```
2. Ejecuta el filtro usando `mpirun` con los hosts definidos en el clúster (ver documentación interna para detalles de SSH y configuración de hosts).

## Notas Adicionales
- Los archivos de entrada deben estar en formato PGM o PPM.
- El Makefile incluye targets para limpiar (`make clean`), pruebas y benchmarking.
- Para benchmarking automatizado, usa el script `run_mpi_all.sh` o el que corresponda.


# Parcial 1 - Programación Paralela
## Procesamiento de Imágenes PPM/PGM

### Descripción
Este proyecto implementa un sistema de procesamiento de imágenes para formatos PPM (Portable Pixmap) y PGM (Portable Graymap) usando programación orientada a objetos en C++. El proyecto incluye implementaciones secuenciales y paralelas para filtros de procesamiento de imágenes.

### Estructura del Proyecto

#### Archivos Principales
- **Image.h/cpp**: Clase base abstracta para manejo de imágenes
- **PGMImage.h/cpp**: Implementación para imágenes en escala de grises (P2)
- **PPMImage.h/cpp**: Implementación para imágenes a color (P3)
- **ImageFactory.h/cpp**: Factory pattern para crear imágenes según su tipo
- **Filter.h/cpp**: Sistema de filtros con diferentes algoritmos
- **processor.cpp**: Aplicación base para lectura y escritura de imágenes
- **filterer.cpp**: Aplicación para aplicar filtros (versión secuencial)

#### Archivos de Configuración
- **Makefile**: Configuración de compilación
- **benchmark_secuencial.sh**: Script para mediciones de rendimiento

### Formatos Soportados

#### PGM (Portable Graymap) - P2
```
P2
# comentarios opcionales
ancho alto
valor_máximo
pixel1 pixel2 pixel3 ...
```

#### PPM (Portable Pixmap) - P3
```
P3
# comentarios opcionales
ancho alto
valor_máximo
r1 g1 b1  r2 g2 b2  r3 g3 b3 ...
```

### Filtros Implementados

1. **Blur (Suavizado)**
   - Kernel de convolución 3x3 con valores 1/9
   - Reduce ruido y suaviza la imagen

2. **Laplace/Laplacian (Detección de bordes)**
   - Kernel Laplaciano estándar
   - Detecta y realza los bordes de la imagen

3. **Sharpen/Sharpening (Realce)**
   - Kernel de realce que aumenta la nitidez
   - Mejora los detalles y contrastes

### Compilación

#### Usando Makefile (si está disponible)
```bash
make all
```

#### Compilación manual
```bash
# Compilar objetos
g++ -Wall -Wextra -std=c++11 -O2 -c Image.cpp
g++ -Wall -Wextra -std=c++11 -O2 -c PGMImage.cpp
g++ -Wall -Wextra -std=c++11 -O2 -c PPMImage.cpp
g++ -Wall -Wextra -std=c++11 -O2 -c ImageFactory.cpp
g++ -Wall -Wextra -std=c++11 -O2 -c Filter.cpp
g++ -Wall -Wextra -std=c++11 -O2 -c processor.cpp
g++ -Wall -Wextra -std=c++11 -O2 -c filterer.cpp

# Enlazar ejecutables
g++ -O2 -o processor processor.o Image.o PGMImage.o PPMImage.o ImageFactory.o
g++ -O2 -o filterer filterer.o Image.o PGMImage.o PPMImage.o ImageFactory.o Filter.o
```

### Uso

#### Procesador Base
```bash
./processor imagen1.ppm imagen2.pgm ...
```

#### Aplicación de Filtros
```bash
./filterer <entrada> <salida> --f <filtro>

# Ejemplos:
./filterer fruit.ppm fruit_blur.ppm --f blur
./filterer lena.pgm lena_sharp.pgm --f sharpen
./filterer puj.pgm puj_edges.pgm --f laplace
```

### Mediciones de Rendimiento

#### Script de Benchmark
```bash
bash benchmark_secuencial.sh
```

El script genera:
- Mediciones detalladas en `resultados_secuencial/tiempos_secuencial.txt`
- Imágenes filtradas en `resultados_secuencial/`
- Resumen de tiempos promedio por filtro

#### Métricas Reportadas
- Tiempo de carga de imagen
- Tiempo de aplicación del filtro
- Tiempo de guardado
- Tiempo total de procesamiento
- Tiempo de CPU
- Tiempo de ejecución total (wall-clock)

### Características Técnicas

#### Paradigma Orientado a Objetos
- Herencia: `PGMImage` y `PPMImage` heredan de `Image`
- Polimorfismo: Métodos virtuales para lectura/escritura
- Factory Pattern: `ImageFactory` para crear instancias
- Strategy Pattern: Sistema de filtros intercambiables

#### Manejo de Memoria
- Allocación dinámica para matrices de píxeles
- Gestión automática en destructores
- Validaciones para prevenir segmentation faults

#### Manejo de Bordes
- Replicación de píxeles del borde para convolución
- Clampeo de valores a rangos válidos [0, maxVal]

### Archivos de Prueba Incluidos
- `test.pgm`: Imagen PGM 4x4 para pruebas básicas
- `test.ppm`: Imagen PPM 3x3 para pruebas básicas
- `fruit.ppm`: Imagen de ejemplo (900x450)
- `lena.ppm`: Imagen clásica de procesamiento (128x128)
- `puj.pgm`: Imagen institucional (1920x600)
- `damma.pgm`: Imagen de ejemplo
- `sulfur.pgm`: Imagen de ejemplo

### Próximas Implementaciones
1. **Versión con Pthreads**: Paralelización por regiones
2. **Versión con OpenMP**: Paralelización de bucles
3. **Versión con MPI**: Distribución entre contenedores Docker


### Notas de Implementación
- Se utilizan arreglos dinámicos en lugar de vectores STL
- Compilación optimizada con -O2
- Mediciones de tiempo en microsegundos
- Soporte completo para comentarios en archivos de imagen
