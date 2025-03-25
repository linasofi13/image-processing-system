#include "buddy_allocator.h"
#include <cstdlib>
#include <iostream>

using namespace std;

// Constructor: asigna un bloque de memoria de tamaño especificado usando malloc.
BuddyAllocator::BuddyAllocator(size_t size) {
    this->size = size;
    memoriaBase = std::malloc(size);
    if (!memoriaBase) {
        cerr << "Error: No se pudo asignar memoria base con Buddy System.\n";
        exit(1);
    }
}

// Destructor: libera el bloque de memoria.
BuddyAllocator::~BuddyAllocator() {
    std::free(memoriaBase);
}

// Asigna un bloque de memoria del tamaño especificado.
// Si el tamaño solicitado supera el bloque disponible, devuelve nullptr.
void* BuddyAllocator::alloc(size_t bytesSolicitados) {
    if (offset + bytesSolicitados > size) {
        std::cerr << "[ERROR] BuddyAllocator sin memoria ("
                  << bytesSolicitados << " bytes solicitados, "
                  << size - offset << " bytes disponibles)\n";
        return nullptr;
    }

    void* ptr = static_cast<unsigned char*>(memoriaBase) + offset;
    offset += bytesSolicitados;
    return ptr;
}


// Libera el bloque de memoria (sin efecto en esta implementación).
void BuddyAllocator::free(void* ptr) {
    // No liberamos porque el Buddy System maneja esto automáticamente.
}