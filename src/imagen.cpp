/// Archivo: imagen.cpp
/// Implementación de la clase Imagen con soporte para Buddy System

#include "imagen.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <chrono>
#include <sys/resource.h>
#include <malloc.h>
#include <cmath>
#include <cstdlib>


using namespace std;
using namespace std::chrono;

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

void Imagen::escalarImagen(float factor) {
    auto inicio = high_resolution_clock::now();
    struct rusage usage_before, usage_after;
    getrusage(RUSAGE_SELF, &usage_before);
    struct mallinfo2 mem_before = mallinfo2();

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
            float origX = x / factor;
            float origY = y / factor;
            
            int x1 = static_cast<int>(origX);
            int y1 = static_cast<int>(origY);
            int x2 = std::min(x1 + 1, ancho - 1);
            int y2 = std::min(y1 + 1, alto - 1);
            
            float dx = origX - x1;
            float dy = origY - y1;

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

    if (!allocador) {
        for (int y = 0; y < alto; y++) {
            for (int x = 0; x < ancho; x++) {
                delete[] pixeles[y][x];
            }
            delete[] pixeles[y];
        }
        delete[] pixeles;
    }

    pixeles = nuevosPixeles;
    ancho = nuevoAncho;
    alto = nuevoAlto;

    auto fin = high_resolution_clock::now();
    getrusage(RUSAGE_SELF, &usage_after);
    struct mallinfo2 mem_after = mallinfo2();

    auto duracion = duration_cast<milliseconds>(fin - inicio).count();
    cout << "\n[INFO] Escalado de imagen (factor " << factor << "):" << endl;
    cout << "  Tiempo de procesamiento: " << duracion << " ms" << endl;
    cout << "  Memoria utilizada: " << (mem_after.uordblks - mem_before.uordblks) / 1024.0 << " KB" << endl;
    cout << "  CPU User: " << (usage_after.ru_utime.tv_sec - usage_before.ru_utime.tv_sec) * 1000.0 +
            (usage_after.ru_utime.tv_usec - usage_before.ru_utime.tv_usec) / 1000.0 << " ms" << endl;
    cout << "  CPU System: " << (usage_after.ru_stime.tv_sec - usage_before.ru_stime.tv_sec) * 1000.0 +
            (usage_after.ru_stime.tv_usec - usage_before.ru_stime.tv_usec) / 1000.0 << " ms" << endl;
    cout << "  Nuevas dimensiones: " << ancho << "x" << alto << endl;
}

void Imagen::rotarImagen(double angulo, unsigned char fillColor /*= 0*/) {
    using namespace std;
    using namespace std::chrono;

    auto inicio = high_resolution_clock::now();
    struct rusage usage_before, usage_after;
    getrusage(RUSAGE_SELF, &usage_before);
    struct mallinfo2 mem_before = mallinfo2();

    // Convertir ángulo a radianes
    double angleRad = angulo * M_PI / 180.0;
    double cosTheta = cos(angleRad);
    double sinTheta = sin(angleRad);

    // 1) Calcular bounding box
    // Nota: ancho y alto son int, se usan double en intermedios
    double w = static_cast<double>(ancho);
    double h = static_cast<double>(alto);

    double absCos = std::fabs(cosTheta);
    double absSin = std::fabs(sinTheta);

    int nuevoAncho = static_cast<int>(std::ceil(w * absCos + h * absSin));
    int nuevoAlto  = static_cast<int>(std::ceil(w * absSin + h * absCos));

    // 2) Crear nueva matriz con el bounding box
    unsigned char*** nuevosPixeles;
    if (allocador) {
        nuevosPixeles = reinterpret_cast<unsigned char***>(
            allocador->alloc(nuevoAlto * sizeof(unsigned char**))
        );
        if (!nuevosPixeles) {
            cerr << "Error: No se pudo asignar memoria con BuddyAllocator para rotación." << endl;
            return;
        }
        for (int y = 0; y < nuevoAlto; y++) {
            nuevosPixeles[y] = reinterpret_cast<unsigned char**>(
                allocador->alloc(nuevoAncho * sizeof(unsigned char*))
            );
            for (int x = 0; x < nuevoAncho; x++) {
                nuevosPixeles[y][x] = reinterpret_cast<unsigned char*>(
                    allocador->alloc(canales * sizeof(unsigned char))
                );
                // Inicializar con fillColor
                for (int c = 0; c < canales; c++) {
                    nuevosPixeles[y][x][c] = fillColor;
                }
            }
        }
    } else {
        nuevosPixeles = new unsigned char**[nuevoAlto];
        for (int y = 0; y < nuevoAlto; y++) {
            nuevosPixeles[y] = new unsigned char*[nuevoAncho];
            for (int x = 0; x < nuevoAncho; x++) {
                nuevosPixeles[y][x] = new unsigned char[canales];
                for (int c = 0; c < canales; c++) {
                    nuevosPixeles[y][x][c] = fillColor;
                }
            }
        }
    }

    // 3) Centros: original (cx, cy), nuevo (cx', cy')
    // Ojo: ancho, alto son enteros
    double cx = w / 2.0;      // centro original
    double cy = h / 2.0;
    double cxn = nuevoAncho / 2.0; // centro nueva
    double cyn = nuevoAlto  / 2.0;

    // 4) Para cada pixel (x, y) del nuevo lienzo, hallar (origX, origY)
    for (int ny = 0; ny < nuevoAlto; ny++) {
        for (int nx = 0; nx < nuevoAncho; nx++) {
            // coordenadas relativas al centro del nuevo lienzo
            double dx = nx - cxn;
            double dy = ny - cyn;

            // transformada inversa
            double origX =  cosTheta * dx + sinTheta * dy + cx;
            double origY = -sinTheta * dx + cosTheta * dy + cy;

            // Bilinear o nearest neighbor:
            // Verificamos si cae dentro de la imagen original
            if (origX >= 0.0 && origX < (w - 1) &&
                origY >= 0.0 && origY < (h - 1)) 
            {
                // Bilinear
                int x1 = static_cast<int>(floor(origX));
                int y1 = static_cast<int>(floor(origY));
                int x2 = x1 + 1;
                int y2 = y1 + 1;
                if (x2>=w) x2=w-1;
                if (y2>=h) y2=h-1;

                double fx = origX - x1;
                double fy = origY - y1;
                double fx1 = 1.0 - fx;
                double fy1 = 1.0 - fy;

                for (int c = 0; c < canales; c++) {
                    double p00 = pixeles[y1][x1][c];
                    double p10 = pixeles[y1][x2][c];
                    double p01 = pixeles[y2][x1][c];
                    double p11 = pixeles[y2][x2][c];

                    double interp = (fx1 * fy1 * p00) + (fx * fy1 * p10) +
                    (fx1 * fy * p01) + (fx * fy * p11);

                    nuevosPixeles[ny][nx][c] = static_cast<unsigned char>(std::round(interp));
                }
            }
            // else: se queda fillColor
        }
    }

    // Liberar la imagen original si NO es Buddy
    if (!allocador) {
        if (pixeles) {
            for (int y = 0; y < alto; y++) {
                for (int x = 0; x < ancho; x++) {
                    delete[] pixeles[y][x];
                }
                delete[] pixeles[y];
            }
            delete[] pixeles;
        }
    }

    // Actualizar puntero y dimensiones
    pixeles = nuevosPixeles;
    ancho   = nuevoAncho;
    alto    = nuevoAlto;

    // 5) Métricas de tiempo y memoria
    auto fin = high_resolution_clock::now();
    getrusage(RUSAGE_SELF, &usage_after);
    struct mallinfo2 mem_after = mallinfo2();
    auto duracion = duration_cast<milliseconds>(fin - inicio).count();

    cout << "\n[INFO] Rotación de imagen (ángulo " << angulo << " grados):" << endl;
    cout << "  Tiempo de procesamiento: " << duracion << " ms" << endl;
    cout << "  Memoria utilizada: " << (mem_after.uordblks - mem_before.uordblks) / 1024.0 << " KB" << endl;
    cout << "  CPU User: " << (usage_after.ru_utime.tv_sec - usage_before.ru_utime.tv_sec) * 1000.0 +
            (usage_after.ru_utime.tv_usec - usage_before.ru_utime.tv_usec) / 1000.0 << " ms" << endl;
    cout << "  CPU System: " << (usage_after.ru_stime.tv_sec - usage_before.ru_stime.tv_sec) * 1000.0 +
            (usage_after.ru_stime.tv_usec - usage_before.ru_stime.tv_usec) / 1000.0 << " ms" << endl;
    cout << "  Nuevas dimensiones: " << ancho << " x " << alto << endl;
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
