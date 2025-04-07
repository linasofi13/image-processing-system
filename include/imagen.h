#ifndef IMAGEN_H
#define IMAGEN_H
#include "buddy_allocator.h"  
#include <string>

class Imagen {
public:
    Imagen(const std::string& rutaArchivo, BuddyAllocator* allocador = nullptr);
    ~Imagen();

    bool cargar();
    void mostrarInformacion() const;

    void escalarImagen(float factor); 
    void rotarImagen(double angulo, unsigned char fillColor = 0); // New method for scaling

    void guardarImagen(const std::string& ruta) const;

private:
    int ancho;
    int alto;
    int canales;
    unsigned char*** pixeles;
    std::string ruta;
    BuddyAllocator* allocador = nullptr; // <-- guarda el puntero para saber si usar Buddy
};

#endif
