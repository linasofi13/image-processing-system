CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

SRC = src/main.cpp src/imagen.cpp src/buddy_allocator.cpp src/stb_wrapper.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = build/image-processing-system

# Directorios
BUILD_DIR = build
OUTPUT_DIR = output

# Imagen de prueba por defecto
TEST_IMG = test/testImg/test.jpg

# Argumentos por defecto para diferentes operaciones
ARGS_INVERTIR = $(TEST_IMG) $(OUTPUT_DIR)/salida_invertida.png invertir -buddy
ARGS_ESCALAR_2X = $(TEST_IMG) $(OUTPUT_DIR)/salida_2x.png escalar 2.0 -buddy
ARGS_ESCALAR_MITAD = $(TEST_IMG) $(OUTPUT_DIR)/salida_mitad.png escalar 0.5 -buddy

# Puedes pasar argumentos personalizados desde línea de comandos con ARGS=
ARGS ?= $(ARGS_INVERTIR)

all: directories $(TARGET)

directories:
	mkdir -p $(BUILD_DIR) $(OUTPUT_DIR)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)
	rm -f $(OUTPUT_DIR)/*.png

# Objetivos para diferentes operaciones
invertir: $(TARGET)
	./$(TARGET) $(ARGS_INVERTIR)

escalar_2x: $(TARGET)
	./$(TARGET) $(ARGS_ESCALAR_2X)

escalar_mitad: $(TARGET)
	./$(TARGET) $(ARGS_ESCALAR_MITAD)

# Objetivo genérico para ejecutar con argumentos personalizados
run: $(TARGET)
	./$(TARGET) $(ARGS)

# Objetivo para ejecutar todas las operaciones de ejemplo
test_all: invertir escalar_2x escalar_mitad
	@echo "Todas las operaciones de prueba completadas."
	@echo "Las imágenes resultantes están en el directorio $(OUTPUT_DIR)/"

.PHONY: all clean directories run invertir escalar_2x escalar_mitad test_all

# Ejemplos de uso:
# make run ARGS="entrada.jpg output/salida.png invertir -buddy"
# make run ARGS="entrada.jpg output/salida_2x.png escalar 2.0 -buddy"
# make invertir
# make escalar_2x
# make escalar_mitad
# make test_all
