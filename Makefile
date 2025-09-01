# ============================================================================
# MAKEFILE PARA PROYECTO DE PROGRAMACIÓN PARALELA
# Filtrado de Imágenes con Multiple Tecnologías Paralelas
# ============================================================================

# Compiladores y configuración
CXX = g++
MPICXX = mpic++

# Flags de compilación
CXXFLAGS = -Wall -Wextra -std=c++11 -O2 -g
DEBUGFLAGS = -DDEBUG -g -O0
RELEASEFLAGS = -O3 -DNDEBUG
OMPFLAGS = -fopenmp
PTHREADFLAGS = -lpthread
MPIFLAGS = 

# Colores para output del makefile
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
PURPLE = \033[0;35m
CYAN = \033[0;36m
NC = \033[0m # No Color

# Targets principales
TARGET = processor
FILTERER_TARGET = filterer
PTH_TARGET = pth_filterer
OMP_TARGET = omp_filterer
MPI_TARGET = mpi_filterer

# Archivos fuente por categoría
CORE_SOURCES = Image.cpp PGMImage.cpp PPMImage.cpp ImageFactory.cpp Filter.cpp
PROCESSOR_SOURCES = processor.cpp $(CORE_SOURCES)
FILTERER_SOURCES = filterer.cpp $(CORE_SOURCES)
PTH_SOURCES = pth_filterer.cpp PthreadFilter.cpp $(CORE_SOURCES)
OMP_SOURCES = omp_filterer.cpp OMPFilter.cpp $(CORE_SOURCES)
MPI_SOURCES = mpi_filterer.cpp MPIFilter.cpp $(CORE_SOURCES)

# Archivos objeto
PROCESSOR_OBJECTS = $(PROCESSOR_SOURCES:.cpp=.o)
FILTERER_OBJECTS = $(FILTERER_SOURCES:.cpp=.o)
PTH_OBJECTS = $(PTH_SOURCES:.cpp=.o)
OMP_OBJECTS = $(OMP_SOURCES:.cpp=.o)
MPI_OBJECTS = $(MPI_SOURCES:.cpp=.o)

# Headers de dependencia
HEADERS = Image.h PGMImage.h PPMImage.h ImageFactory.h Filter.h PthreadFilter.h OMPFilter.h MPIFilter.h

# Directorios
BUILD_DIR = build
TEST_DIR = test_images
RESULTS_DIR = results
BENCHMARK_DIR = benchmark_results

# ============================================================================
# REGLAS PRINCIPALES
# ============================================================================

.PHONY: all clean clean-all help test benchmark install debug release setup

# Regla por defecto - compila todas las versiones
all: banner setup $(TARGET) $(FILTERER_TARGET) $(PTH_TARGET) $(OMP_TARGET) $(MPI_TARGET)
	@echo "$(GREEN)✅ Todas las versiones compiladas exitosamente$(NC)"
	@echo "$(BLUE)📦 Ejecutables disponibles:$(NC)"
	@echo "   🔸 $(TARGET) - Procesador original"
	@echo "   🔸 $(FILTERER_TARGET) - Versión secuencial"
	@echo "   🔸 $(PTH_TARGET) - Versión Pthreads"
	@echo "   🔸 $(OMP_TARGET) - Versión OpenMP" 
	@echo "   🔸 $(MPI_TARGET) - Versión MPI"

# Banner informativo
banner:
	@echo "$(CYAN)============================================================================$(NC)"
	@echo "$(CYAN)  PROYECTO DE PROGRAMACIÓN PARALELA - FILTRADO DE IMÁGENES$(NC)"
	@echo "$(CYAN)  Compilando implementaciones: Secuencial, Pthreads, OpenMP y MPI$(NC)"
	@echo "$(CYAN)============================================================================$(NC)"

# Configuración inicial de directorios
setup:
	@mkdir -p $(BUILD_DIR) $(RESULTS_DIR) $(BENCHMARK_DIR)

# ============================================================================
# COMPILACIÓN DE EJECUTABLES
# ============================================================================

# Ejecutable original (processor)
$(TARGET): $(PROCESSOR_OBJECTS)
	@echo "$(YELLOW)🔨 Compilando $(TARGET)...$(NC)"
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(PROCESSOR_OBJECTS)
	@echo "$(GREEN)✅ $(TARGET) compilado$(NC)"

# Ejecutable secuencial (filterer)
$(FILTERER_TARGET): $(FILTERER_OBJECTS)
	@echo "$(YELLOW)🔨 Compilando $(FILTERER_TARGET) (Secuencial)...$(NC)"
	$(CXX) $(CXXFLAGS) -o $(FILTERER_TARGET) $(FILTERER_OBJECTS)
	@echo "$(GREEN)✅ $(FILTERER_TARGET) compilado$(NC)"

# Ejecutable con Pthreads
$(PTH_TARGET): $(PTH_OBJECTS)
	@echo "$(YELLOW)🔨 Compilando $(PTH_TARGET) (Pthreads)...$(NC)"
	$(CXX) $(CXXFLAGS) -o $(PTH_TARGET) $(PTH_OBJECTS) $(PTHREADFLAGS)
	@echo "$(GREEN)✅ $(PTH_TARGET) compilado$(NC)"

# Ejecutable con OpenMP
$(OMP_TARGET): $(OMP_OBJECTS)
	@echo "$(YELLOW)🔨 Compilando $(OMP_TARGET) (OpenMP)...$(NC)"
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) -o $(OMP_TARGET) $(OMP_OBJECTS)
	@echo "$(GREEN)✅ $(OMP_TARGET) compilado$(NC)"

# Ejecutable con MPI
$(MPI_TARGET): $(MPI_OBJECTS)
	@echo "$(YELLOW)🔨 Compilando $(MPI_TARGET) (MPI)...$(NC)"
	$(MPICXX) $(CXXFLAGS) -o $(MPI_TARGET) $(MPI_OBJECTS) $(MPIFLAGS)
	@echo "$(GREEN)✅ $(MPI_TARGET) compilado$(NC)"

# ============================================================================
# REGLAS DE COMPILACIÓN DE OBJETOS
# ============================================================================

# Reglas específicas para objetos con diferentes flags

# Objetos para versión OpenMP
omp_filterer.o: omp_filterer.cpp
	@echo "$(BLUE)🔧 Compilando $< (OpenMP)...$(NC)"
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) -c $< -o $@

OMPFilter.o: OMPFilter.cpp OMPFilter.h
	@echo "$(BLUE)🔧 Compilando $< (OpenMP)...$(NC)"
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) -c $< -o $@

# Objetos para versión Pthreads
pth_filterer.o: pth_filterer.cpp
	@echo "$(BLUE)🔧 Compilando $< (Pthreads)...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

PthreadFilter.o: PthreadFilter.cpp PthreadFilter.h
	@echo "$(BLUE)🔧 Compilando $< (Pthreads)...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Objetos para versión MPI
mpi_filterer.o: mpi_filterer.cpp
	@echo "$(BLUE)🔧 Compilando $< (MPI)...$(NC)"
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

MPIFilter.o: MPIFilter.cpp MPIFilter.h
	@echo "$(BLUE)🔧 Compilando $< (MPI)...$(NC)"
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

# Regla general para otros objetos
%.o: %.cpp %.h
	@echo "$(BLUE)🔧 Compilando $<...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para archivos .cpp sin .h correspondiente
%.o: %.cpp
	@echo "$(BLUE)🔧 Compilando $<...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================================================
# TARGETS DE TESTING Y BENCHMARKING
# ============================================================================

# Testing básico de correctitud
test: all
	@echo "$(PURPLE)🧪 Ejecutando tests de correctitud...$(NC)"
	@mkdir -p $(RESULTS_DIR)
	@echo "$(CYAN)Testing con imagen pequeña (test.pgm)...$(NC)"
	./$(FILTERER_TARGET) test.pgm $(RESULTS_DIR)/test_seq.pgm --f blur
	./$(PTH_TARGET) test.pgm $(RESULTS_DIR)/test_pth.pgm --f blur
	./$(OMP_TARGET) test.pgm $(RESULTS_DIR)/test_omp.pgm --f blur
	mpirun -np 2 ./$(MPI_TARGET) test.pgm $(RESULTS_DIR)/test_mpi.pgm --f blur
	@echo "$(CYAN)Verificando checksums...$(NC)"
	@md5sum $(RESULTS_DIR)/test_*.pgm
	@echo "$(GREEN)✅ Tests de correctitud completados$(NC)"

# Benchmark completo
benchmark: all
	@echo "$(PURPLE)📊 Ejecutando benchmark completo...$(NC)"
	@mkdir -p $(BENCHMARK_DIR)
	@if [ -f benchmark_completo.sh ]; then \
		chmod +x benchmark_completo.sh; \
		./benchmark_completo.sh; \
	else \
		echo "$(RED)❌ Script benchmark_completo.sh no encontrado$(NC)"; \
	fi
	@echo "$(GREEN)✅ Benchmark completado$(NC)"

# Testing rápido con imagen pequeña
quick-test: all
	@echo "$(PURPLE)⚡ Test rápido de funcionalidad...$(NC)"
	@mkdir -p $(RESULTS_DIR)
	./$(FILTERER_TARGET) test.pgm $(RESULTS_DIR)/quick_seq.pgm --f blur
	./$(OMP_TARGET) test.pgm $(RESULTS_DIR)/quick_omp.pgm --f blur
	@echo "$(CYAN)Comparando resultados...$(NC)"
	@if cmp -s $(RESULTS_DIR)/quick_seq.pgm $(RESULTS_DIR)/quick_omp.pgm; then \
		echo "$(GREEN)✅ Resultados idénticos$(NC)"; \
	else \
		echo "$(RED)❌ Resultados difieren$(NC)"; \
	fi

# ============================================================================
# TARGETS DE DESARROLLO
# ============================================================================

# Compilación en modo debug
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: all
	@echo "$(YELLOW)🐛 Versión debug compilada$(NC)"

# Compilación optimizada para release
release: CXXFLAGS += $(RELEASEFLAGS)
release: all
	@echo "$(GREEN)🚀 Versión release optimizada compilada$(NC)"

# Verificación de dependencias
check-deps:
	@echo "$(PURPLE)🔍 Verificando dependencias...$(NC)"
	@echo "$(CYAN)Verificando compiladores:$(NC)"
	@which g++ > /dev/null && echo "✅ g++ disponible" || echo "❌ g++ no encontrado"
	@which mpic++ > /dev/null && echo "✅ mpic++ disponible" || echo "❌ mpic++ no encontrado"
	@echo "$(CYAN)Verificando soporte OpenMP:$(NC)"
	@echo '#include <omp.h>\nint main(){return omp_get_num_threads();}' | g++ -fopenmp -x c++ - -o /tmp/omp_test 2>/dev/null && echo "✅ OpenMP disponible" || echo "❌ OpenMP no disponible"
	@rm -f /tmp/omp_test
	@echo "$(CYAN)Verificando imágenes de prueba:$(NC)"
	@test -f test.pgm && echo "✅ test.pgm encontrado" || echo "❌ test.pgm no encontrado"
	@test -f sulfur.pgm && echo "✅ sulfur.pgm encontrado" || echo "❌ sulfur.pgm no encontrado"

# Instalación de dependencias (Ubuntu/Debian)
install-deps:
	@echo "$(PURPLE)📦 Instalando dependencias del sistema...$(NC)"
	sudo apt-get update
	sudo apt-get install -y build-essential libopenmpi-dev openmpi-bin

# ============================================================================
# TARGETS DE LIMPIEZA Y MANTENIMIENTO
# ============================================================================

# Limpieza básica (objetos y ejecutables)
clean:
	@echo "$(RED)🧹 Limpiando archivos compilados...$(NC)"
	rm -f *.o $(TARGET) $(FILTERER_TARGET) $(PTH_TARGET) $(OMP_TARGET) $(MPI_TARGET)
	@echo "$(GREEN)✅ Limpieza completada$(NC)"

# Limpieza completa (incluye resultados y directorios)
clean-all: clean
	@echo "$(RED)🗑️  Limpieza completa...$(NC)"
	rm -rf $(BUILD_DIR) $(RESULTS_DIR) $(BENCHMARK_DIR)
	rm -f *_blur.pgm *_blur.ppm *_laplace.pgm *_sharpen.ppm
	rm -f results_*.pgm output_*.pgm output_*.ppm
	@echo "$(GREEN)✅ Limpieza completa finalizada$(NC)"

# Limpieza solo de resultados
clean-results:
	@echo "$(YELLOW)🧹 Limpiando solo resultados...$(NC)"
	rm -rf $(RESULTS_DIR) $(BENCHMARK_DIR)
	rm -f *_blur.pgm *_blur.ppm *_laplace.pgm *_sharpen.ppm
	rm -f results_*.pgm output_*.pgm output_*.ppm

# ============================================================================
# TARGETS DE INFORMACIÓN Y AYUDA
# ============================================================================

# Ayuda del Makefile
help:
	@echo "$(CYAN)============================================================================$(NC)"
	@echo "$(CYAN)  MAKEFILE PARA PROYECTO DE PROGRAMACIÓN PARALELA$(NC)"
	@echo "$(CYAN)============================================================================$(NC)"
	@echo ""
	@echo "$(YELLOW)TARGETS PRINCIPALES:$(NC)"
	@echo "  $(GREEN)all$(NC)           - Compila todas las versiones (por defecto)"
	@echo "  $(GREEN)clean$(NC)         - Elimina archivos compilados"
	@echo "  $(GREEN)clean-all$(NC)     - Limpieza completa del proyecto"
	@echo "  $(GREEN)help$(NC)          - Muestra esta ayuda"
	@echo ""
	@echo "$(YELLOW)COMPILACIÓN INDIVIDUAL:$(NC)"
	@echo "  $(GREEN)$(TARGET)$(NC)     - Compilar solo el procesador original"
	@echo "  $(GREEN)$(FILTERER_TARGET)$(NC)      - Compilar solo la versión secuencial"
	@echo "  $(GREEN)$(PTH_TARGET)$(NC) - Compilar solo la versión Pthreads"
	@echo "  $(GREEN)$(OMP_TARGET)$(NC) - Compilar solo la versión OpenMP"
	@echo "  $(GREEN)$(MPI_TARGET)$(NC) - Compilar solo la versión MPI"
	@echo ""
	@echo "$(YELLOW)TESTING Y BENCHMARKING:$(NC)"
	@echo "  $(GREEN)test$(NC)          - Ejecuta tests de correctitud"
	@echo "  $(GREEN)quick-test$(NC)    - Test rápido de funcionalidad"
	@echo "  $(GREEN)benchmark$(NC)     - Ejecuta benchmark completo"
	@echo ""
	@echo "$(YELLOW)DESARROLLO:$(NC)"
	@echo "  $(GREEN)debug$(NC)         - Compila en modo debug"
	@echo "  $(GREEN)release$(NC)       - Compila optimizado para release"
	@echo "  $(GREEN)check-deps$(NC)    - Verifica dependencias del sistema"
	@echo "  $(GREEN)install-deps$(NC)  - Instala dependencias (requiere sudo)"
	@echo ""
	@echo "$(YELLOW)EJEMPLO DE USO:$(NC)"
	@echo "  make all           # Compila todo"
	@echo "  make test          # Ejecuta tests"
	@echo "  make benchmark     # Ejecuta benchmark"
	@echo "  make clean         # Limpia archivos"
	@echo ""

# Información del proyecto
info:
	@echo "$(CYAN)============================================================================$(NC)"
	@echo "$(CYAN)  INFORMACIÓN DEL PROYECTO$(NC)"
	@echo "$(CYAN)============================================================================$(NC)"
	@echo "$(YELLOW)Proyecto:$(NC) Análisis de Programación Paralela en Filtrado de Imágenes"
	@echo "$(YELLOW)Tecnologías:$(NC) C++11, Pthreads, OpenMP, MPI"
	@echo "$(YELLOW)Compilador:$(NC) GCC con soporte para estándares paralelos"
	@echo "$(YELLOW)Formatos:$(NC) PGM (P2), PPM (P3)"
	@echo "$(YELLOW)Filtros:$(NC) Blur, Laplacian, Sharpen"
	@echo ""
	@echo "$(YELLOW)ARCHIVOS PRINCIPALES:$(NC)"
	@ls -la *.cpp *.h 2>/dev/null || echo "Archivos fuente no encontrados"
	@echo ""
	@echo "$(YELLOW)EJECUTABLES COMPILADOS:$(NC)"
	@ls -la $(TARGET) $(FILTERER_TARGET) $(PTH_TARGET) $(OMP_TARGET) $(MPI_TARGET) 2>/dev/null || echo "No hay ejecutables compilados"

# Lista archivos de imagen disponibles
list-images:
	@echo "$(PURPLE)📸 Imágenes disponibles para testing:$(NC)"
	@ls -la *.pgm *.ppm 2>/dev/null || echo "No se encontraron imágenes"

# ============================================================================
# TARGETS ESPECIALES
# ============================================================================

# Reconstrucción completa
rebuild: clean all

# Verificación de integridad del código
lint:
	@echo "$(PURPLE)🔍 Verificando estilo de código...$(NC)"
	@which cppcheck > /dev/null 2>&1 && cppcheck --enable=all --std=c++11 *.cpp || echo "$(YELLOW)cppcheck no disponible$(NC)"

# Backup del proyecto
backup:
	@echo "$(PURPLE)💾 Creando backup del proyecto...$(NC)"
	tar -czf backup_$(shell date +%Y%m%d_%H%M%S).tar.gz *.cpp *.h *.pgm *.ppm Makefile README.md *.sh 2>/dev/null
	@echo "$(GREEN)✅ Backup creado$(NC)"

# Análisis de tamaño de ejecutables
size-analysis: all
	@echo "$(PURPLE)📏 Análisis de tamaño de ejecutables:$(NC)"
	@ls -lh $(TARGET) $(FILTERER_TARGET) $(PTH_TARGET) $(OMP_TARGET) $(MPI_TARGET) 2>/dev/null || echo "Algunos ejecutables no están compilados"
	@echo ""
	@echo "$(PURPLE)🔍 Símbolos y secciones:$(NC)"
	@size $(TARGET) $(FILTERER_TARGET) $(PTH_TARGET) $(OMP_TARGET) $(MPI_TARGET) 2>/dev/null || echo "No se puede analizar el tamaño"

# ============================================================================
# DEPENDENCIAS
# ============================================================================

# Dependencias automáticas de headers
$(PROCESSOR_OBJECTS): $(HEADERS)
$(FILTERER_OBJECTS): $(HEADERS) 
$(PTH_OBJECTS): $(HEADERS)
$(OMP_OBJECTS): $(HEADERS)
$(MPI_OBJECTS): $(HEADERS)

# Forzar recompilación si el Makefile cambia
$(PROCESSOR_OBJECTS) $(FILTERER_OBJECTS) $(PTH_OBJECTS) $(OMP_OBJECTS) $(MPI_OBJECTS): Makefile

# ============================================================================
# PHONY TARGETS
# ============================================================================

.PHONY: all clean clean-all clean-results help test benchmark quick-test debug release \
        setup banner check-deps install-deps info list-images rebuild lint backup \
        size-analysis