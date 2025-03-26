/// Archivo: imagen.cpp
/// Implementación de la clase Imagen con soporte para Buddy System

#include "imagen.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>

using namespace std;

// Constructor
Imagen::Imagen(const std::string& rutaArchivo, BuddyAllocator* allocador)
    : ancho(0), alto(0), canales(0), pixeles(nullptr), ruta(rutaArchivo), allocador(allocador) {}

// Destructor
Imagen::~Imagen() {
    // Solo liberar si usamos new/delete (el Buddy System no libera manualmente)
    if (!allocador && pixeles) {
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                delete[] pixeles[y][x];
            }
            delete[] pixeles[y];
        }
        delete[] pixeles;
    }
}

// Cargar imagen desde archivo y convertir a matriz tridimensional
bool Imagen::cargar() {
    unsigned char* datos = stbi_load(ruta.c_str(), &ancho, &alto, &canales, 0);
    if (!datos) {
        cerr << "Error al cargar la imagen: " << ruta << endl;
        return false;
    }

    cout << "[OK] Imagen cargada desde: " << ruta << endl;

    // Reservar espacio para pixeles[alto]
    if (allocador) {
        pixeles = reinterpret_cast<unsigned char***>(
            allocador->alloc(alto * sizeof(unsigned char**))
        );
        if (!pixeles) {
            cerr << "Error: No se pudo asignar memoria con BuddyAllocator para pixeles." << endl;
            stbi_image_free(datos);
            return false;
        }
    } else {
        pixeles = new unsigned char**[alto];
    }

    int indice = 0;

    for (int y = 0; y < alto; y++) {
        // pixeles[y] ← ancho
        if (allocador) {
            pixeles[y] = reinterpret_cast<unsigned char**>(
                allocador->alloc(ancho * sizeof(unsigned char*))
            );
            if (!pixeles[y]) {
                cerr << "Error: Falla al asignar fila " << y << " con BuddyAllocator." << endl;
                stbi_image_free(datos);
                return false;
            }
        } else {
            pixeles[y] = new unsigned char*[ancho];
        }

        for (int x = 0; x < ancho; x++) {
            // pixeles[y][x] ← canales
            if (allocador) {
                pixeles[y][x] = reinterpret_cast<unsigned char*>(
                    allocador->alloc(canales * sizeof(unsigned char))
                );
                if (!pixeles[y][x]) {
                    cerr << "Error: Falla al asignar columna " << x << " con BuddyAllocator." << endl;
                    stbi_image_free(datos);
                    return false;
                }
            } else {
                pixeles[y][x] = new unsigned char[canales];
            }

            // Copiar los valores RGB (o RGBA)
            for (int c = 0; c < canales; c++) {
                pixeles[y][x][c] = datos[indice++];
            }
        }
    }

    stbi_image_free(datos);
    return true;
}

// Mostrar dimensiones y canales
void Imagen::mostrarInformacion() const {
    cout << "Dimensiones: " << ancho << " x " << alto << endl;
    cout << "Canales: " << canales << endl;
}

void Imagen::invertirColores() {
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            for (int c = 0; c < canales; c++) {
                pixeles[y][x][c] = 255 - pixeles[y][x][c];
            }
        }
    }
}

void Imagen::escalarImagen(float factor) {
    int nuevoAncho = static_cast<int>(ancho * factor);
    int nuevoAlto = static_cast<int>(alto * factor);
    
    // Crear nueva matriz para la imagen escalada
    unsigned char*** nuevosPixeles;
    if (allocador) {
        nuevosPixeles = reinterpret_cast<unsigned char***>(
            allocador->alloc(nuevoAlto * sizeof(unsigned char**))
        );
    } else {
        nuevosPixeles = new unsigned char**[nuevoAlto];
    }

    // Asignar memoria para las filas y columnas
    for (int y = 0; y < nuevoAlto; y++) {
        if (allocador) {
            nuevosPixeles[y] = reinterpret_cast<unsigned char**>(
                allocador->alloc(nuevoAncho * sizeof(unsigned char*))
            );
        } else {
            nuevosPixeles[y] = new unsigned char*[nuevoAncho];
        }

        for (int x = 0; x < nuevoAncho; x++) {
            if (allocador) {
                nuevosPixeles[y][x] = reinterpret_cast<unsigned char*>(
                    allocador->alloc(canales * sizeof(unsigned char))
                );
            } else {
                nuevosPixeles[y][x] = new unsigned char[canales];
            }
        }
    }

    // Realizar el escalado usando interpolación bilineal
    for (int y = 0; y < nuevoAlto; y++) {
        for (int x = 0; x < nuevoAncho; x++) {
            // Calcular las coordenadas correspondientes en la imagen original
            float origX = x / factor;
            float origY = y / factor;
            
            // Obtener las coordenadas de los píxeles vecinos
            int x1 = static_cast<int>(origX);
            int y1 = static_cast<int>(origY);
            int x2 = std::min(x1 + 1, ancho - 1);
            int y2 = std::min(y1 + 1, alto - 1);
            
            // Calcular los pesos para la interpolación
            float dx = origX - x1;
            float dy = origY - y1;

            // Interpolar para cada canal
            for (int c = 0; c < canales; c++) {
                float valor = 
                    pixeles[y1][x1][c] * (1 - dx) * (1 - dy) +
                    pixeles[y1][x2][c] * dx * (1 - dy) +
                    pixeles[y2][x1][c] * (1 - dx) * dy +
                    pixeles[y2][x2][c] * dx * dy;
                
                nuevosPixeles[y][x][c] = static_cast<unsigned char>(valor);
            }
        }
    }

    // Liberar memoria antigua si no estamos usando el allocador
    if (!allocador) {
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                delete[] pixeles[y][x];
            }
            delete[] pixeles[y];
        }
        delete[] pixeles;
    }

    // Actualizar los atributos de la imagen
    pixeles = nuevosPixeles;
    ancho = nuevoAncho;
    alto = nuevoAlto;
}

void Imagen::guardarImagen(const std::string& nombreArchivo) const {
    unsigned char* buffer = new unsigned char[alto * ancho * canales];
    int index = 0;

    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            for (int c = 0; c < canales; c++) {
                buffer[index++] = pixeles[y][x][c];
            }
        }
    }

    stbi_write_png(nombreArchivo.c_str(), ancho, alto, canales, buffer, ancho * canales);
    delete[] buffer;

    std::cout << "[OK] Imagen guardada en: " << nombreArchivo << std::endl;
}
