#include <iostream>
#include <chrono>
#include <cstring>
#include "imagen.h"
#include "buddy_allocator.h"

using namespace std;
using namespace std::chrono;

void mostrarUso(const char* nombrePrograma) {
    cout << "Uso: " << nombrePrograma << " <imagen_entrada> <imagen_salida> <-buddy | -no-buddy>" << endl;
    cout << "Ejemplo: " << nombrePrograma << " test/testImg/test.jpg salida.png -buddy" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Error: Número incorrecto de argumentos." << endl;
        mostrarUso(argv[0]);
        return 1;
    }

    string rutaEntrada = argv[1];
    string rutaSalida = argv[2];
    string modo = argv[3];

    bool usarBuddy = false;
    if (modo == "-buddy") {
        usarBuddy = true;
    } else if (modo == "-no-buddy") {
        usarBuddy = false;
    } else {
        cerr << "Error: Modo inválido. Usa -buddy o -no-buddy." << endl;
        mostrarUso(argv[0]);
        return 1;
    }

    auto inicio = high_resolution_clock::now();

    if (usarBuddy) {
        cout << "[INFO] Modo seleccionado: Buddy System" << endl;
        BuddyAllocator allocator(128 * 1024 * 1024); // 128 MB

        Imagen imagen(rutaEntrada, &allocator);

        if (!imagen.cargar()) return 1;
        imagen.mostrarInformacion();
        imagen.invertirColores();                    
        imagen.guardarImagen(rutaSalida);             
    } else {
        cout << "[INFO] Modo seleccionado: Convencional (new/delete)" << endl;
        Imagen imagen(rutaEntrada);

        if (!imagen.cargar()) return 1;
        imagen.mostrarInformacion();
        imagen.invertirColores();                    
        imagen.guardarImagen(rutaSalida);             
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio).count();
    cout << "[INFO] Tiempo de procesamiento: " << duracion << " ms" << endl;

    return 0;
}
