#include <iostream>
#include <chrono>
#include <cstring>
#include <filesystem>
#include "imagen.h"
#include "buddy_allocator.h"

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

void mostrarUso(const char* nombrePrograma) {
    cout << "Uso: " << nombrePrograma << " <imagen_entrada> <imagen_salida> <operacion> <parametros> <-buddy | -no-buddy>" << endl;
    cout << "Operaciones disponibles:" << endl;
    cout << "  invertir              - Invierte los colores de la imagen" << endl;
    cout << "  escalar <factor>      - Escala la imagen por el factor especificado (ej: 2.0 para duplicar)" << endl;
    cout << "Ejemplos:" << endl;
    cout << "  " << nombrePrograma << " test/testImg/test.jpg output/salida_invertida.png invertir -buddy" << endl;
    cout << "  " << nombrePrograma << " test/testImg/test.jpg output/salida_2x.png escalar 2.0 -buddy" << endl;
    cout << "  " << nombrePrograma << " test/testImg/test.jpg output/salida_mitad.png escalar 0.5 -no-buddy" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cerr << "Error: Número incorrecto de argumentos." << endl;
        mostrarUso(argv[0]);
        return 1;
    }

    string rutaEntrada = argv[1];
    string rutaSalida = argv[2];
    string operacion = argv[3];
    string modo;
    float factorEscala = 1.0f;

    // Verificar que la carpeta output existe
    if (!fs::exists("output")) {
        fs::create_directory("output");
    }

    // Determinar el modo y los parámetros según la operación
    if (operacion == "invertir") {
        if (argc != 5) {
            cerr << "Error: Número incorrecto de argumentos para invertir." << endl;
            mostrarUso(argv[0]);
            return 1;
        }
        modo = argv[4];
    } else if (operacion == "escalar") {
        if (argc != 6) {
            cerr << "Error: Número incorrecto de argumentos para escalar." << endl;
            mostrarUso(argv[0]);
            return 1;
        }
        try {
            factorEscala = stof(argv[4]);
            if (factorEscala <= 0) {
                cerr << "Error: El factor de escala debe ser mayor que 0." << endl;
                return 1;
            }
        } catch (const exception& e) {
            cerr << "Error: Factor de escala inválido." << endl;
            return 1;
        }
        modo = argv[5];
    } else {
        cerr << "Error: Operación no válida. Use 'invertir' o 'escalar'." << endl;
        mostrarUso(argv[0]);
        return 1;
    }

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
        
        if (operacion == "invertir") {
            cout << "[INFO] Aplicando inversión de colores..." << endl;
            imagen.invertirColores();
        } else { // escalar
            cout << "[INFO] Aplicando escalado con factor " << factorEscala << "..." << endl;
            imagen.escalarImagen(factorEscala);
        }
        
        imagen.guardarImagen(rutaSalida);             
    } else {
        cout << "[INFO] Modo seleccionado: Convencional (new/delete)" << endl;
        Imagen imagen(rutaEntrada);

        if (!imagen.cargar()) return 1;
        imagen.mostrarInformacion();
        
        if (operacion == "invertir") {
            cout << "[INFO] Aplicando inversión de colores..." << endl;
            imagen.invertirColores();
        } else { // escalar
            cout << "[INFO] Aplicando escalado con factor " << factorEscala << "..." << endl;
            imagen.escalarImagen(factorEscala);
        }
        
        imagen.guardarImagen(rutaSalida);             
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio).count();
    cout << "[INFO] Tiempo de procesamiento: " << duracion << " ms" << endl;
    cout << "[INFO] Imagen guardada en: " << rutaSalida << endl;

    return 0;
}
