CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

SRC = src/main.cpp src/imagen.cpp src/buddy_allocator.cpp src/stb_wrapper.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = build/image-processing-system

# Puedes pasar argumentos desde línea de comandos con ARGS=
ARGS = test/testImg/test.jpg salida.png -buddy

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET) $(ARGS)
