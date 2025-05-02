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
    cout << "Uso: " << nombrePrograma << " <imagen_entrada> <imagen_salida> <operacion> [<parametros>] <-buddy | -no-buddy>" << endl;
    cout << "Operaciones disponibles:" << endl;
    cout << "  escalar <factor>      - Escala la imagen por el factor especificado (ej: 2.0 para duplicar)" << endl;
    cout << "  rotar <angulo>        - Rota la imagen en su centro por el ángulo especificado en grados" << endl;
    cout << "Ejemplos:" << endl;
    cout << "  " << nombrePrograma << " entrada.jpg salida_invertida.png invertir -buddy" << endl;
    cout << "  " << nombrePrograma << " entrada.jpg salida_2x.png escalar 2.0 -buddy" << endl;
    cout << "  " << nombrePrograma << " entrada.jpg salida_rotada.png rotar 45 -no-buddy" << endl;
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
    double angulo = 0.0;

    if (!fs::exists("output")) {
        fs::create_directory("output");
    }

    if (operacion == "escalar") {
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
    } else if (operacion == "rotar") {
        if (argc != 6) {
            cerr << "Error: Número incorrecto de argumentos para rotar." << endl;
            mostrarUso(argv[0]);
            return 1;
        }
        try {
            angulo = stod(argv[4]);
        } catch (const exception& e) {
            cerr << "Error: Ángulo inválido." << endl;
            return 1;
        }
        modo = argv[5];
    } else {
        cerr << "Error: Operación no válida. Use 'invertir', 'escalar' o 'rotar'." << endl;
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
        cout << "=== PROCESAMIENTO DE IMAGEN ===" << endl;
        cout << "Archivo de entrada: " << rutaEntrada << endl;
        cout << "Archivo de salida: " << rutaSalida << endl;
        cout << "Modo de asignación de memoria: Buddy System" << endl;
        cout << "------------------------" << endl;
        cout << "[INFO] Procesamiento con Buddy System:" << endl;

        BuddyAllocator allocator(512 * 1024 * 1024);
        Imagen imagenBuddy(rutaEntrada, &allocator);
        if (!imagenBuddy.cargar()) return 1;
        imagenBuddy.mostrarInformacion();

        auto inicioBuddy = high_resolution_clock::now();

        if (operacion == "escalar") {
            imagenBuddy.escalarImagen(factorEscala);
            cout << "[INFO] Imagen escalada correctamente (Buddy System)." << endl;
        } else if (operacion == "rotar") {
            imagenBuddy.rotarImagen(angulo);
            cout << "[INFO] Imagen rotada correctamente (Buddy System)." << endl;
        }

        auto finBuddy = high_resolution_clock::now();
        auto duracionBuddy = duration_cast<milliseconds>(finBuddy - inicioBuddy).count();
        imagenBuddy.guardarImagen(rutaSalida);

        cout << "------------------------" << endl;
        cout << "[INFO] Procesamiento con sistema convencional (new/delete):" << endl;

        Imagen imagenConvencional(rutaEntrada);
        if (!imagenConvencional.cargar()) return 1;

        auto inicioConvencional = high_resolution_clock::now();

        if (operacion == "escalar") {
            imagenConvencional.escalarImagen(factorEscala);
            cout << "[INFO] Imagen escalada correctamente (Convencional)." << endl;
        } else if (operacion == "rotar") {
            imagenConvencional.rotarImagen(angulo);
            cout << "[INFO] Imagen rotada correctamente (Convencional)." << endl;
        }

        auto finConvencional = high_resolution_clock::now();
        auto duracionConvencional = duration_cast<milliseconds>(finConvencional - inicioConvencional).count();

        cout << "------------------------" << endl;
        cout << "TIEMPO DE PROCESAMIENTO:" << endl;
        cout << " - Sin Buddy System: " << duracionConvencional << " ms" << endl;
        cout << " - Con Buddy System: " << duracionBuddy << " ms" << endl;
        cout << "------------------------" << endl;
        cout << "[INFO] Imagen guardada correctamente en " << rutaSalida << endl;
    } else {
        cout << "=== PROCESAMIENTO DE IMAGEN ===" << endl;
        cout << "Archivo de entrada: " << rutaEntrada << endl;
        cout << "Archivo de salida: " << rutaSalida << endl;
        cout << "Modo de asignación de memoria: Convencional (new/delete)" << endl;
        cout << "------------------------" << endl;

        Imagen imagen(rutaEntrada);
        if (!imagen.cargar()) return 1;
        imagen.mostrarInformacion();

        auto inicio = high_resolution_clock::now();

        if (operacion == "escalar") {
            imagen.escalarImagen(factorEscala);
            cout << "[INFO] Imagen escalada correctamente." << endl;
        } else if (operacion == "rotar") {
            imagen.rotarImagen(angulo);
            cout << "[INFO] Imagen rotada correctamente." << endl;
        }

        auto fin = high_resolution_clock::now();
        auto duracion = duration_cast<milliseconds>(fin - inicio).count();

        cout << "------------------------" << endl;
        cout << "TIEMPO DE PROCESAMIENTO: " << duracion << " ms" << endl;
        cout << "------------------------" << endl;
        cout << "[INFO] Imagen guardada correctamente en " << rutaSalida << endl;
    }

    auto fin = high_resolution_clock::now();
    auto duracion = duration_cast<milliseconds>(fin - inicio).count();
    cout << "[INFO] Tiempo de procesamiento: " << duracion << " ms" << endl;
    cout << "[INFO] Imagen guardada en: " << rutaSalida << endl;

    return 0;
}
