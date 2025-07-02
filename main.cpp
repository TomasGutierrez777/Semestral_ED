#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

#include "algoritmos/kmp.cpp"
#include "algoritmos/boyermoore.cpp"
#include "algoritmos/rabinkarp.cpp"

namespace fs = std::filesystem;

std::vector<std::pair<std::string, std::string>> leerArchivosRecursivos(const std::string& carpeta) {
    std::vector<std::pair<std::string, std::string>> archivos;
    for (const auto& entry : fs::recursive_directory_iterator(carpeta)) {
        if (entry.is_regular_file()) {
            std::ifstream archivo(entry.path());
            if (!archivo.is_open()) continue;

            std::string contenido, linea;
            while (getline(archivo, linea)) {
                contenido += linea + "\n";
            }
            archivos.emplace_back(entry.path().string(), contenido);
        }
    }
    return archivos;
}

int main() {
    std::string carpeta = "documentos";
    std::string patron;
    std::cout << "Ingrese patron a buscar: ";
    std::getline(std::cin, patron);

    auto archivos = leerArchivosRecursivos(carpeta);

    std::cout << "\n=== RESULTADOS Boyer-Moore ===\n";
    for (const auto& [ruta, contenido] : archivos) {
        auto posiciones = boyerMooreSearch(contenido, patron);
        if (!posiciones.empty()) {
            std::cout << "BM encontro en: " << ruta << " posiciones: ";
            for (int p : posiciones) std::cout << p << " ";
            std::cout << std::endl;
        }
    }
    
    std::cout << "\n=== RESULTADOS Knuth-Morris-Pratt ===\n";
    for (const auto& [ruta, contenido] : archivos) {
        auto posiciones = kmpSearch(contenido, patron);
        if (!posiciones.empty()) {
            std::cout << "KMP encontro en: " << ruta << " posiciones: ";
            for (int p : posiciones) std::cout << p << " ";
            std::cout << std::endl;
        }
    }

    std::cout << "\n=== RESULTADOS Rabin-Karp ===\n";
    for (const auto& [ruta, contenido] : archivos) {
        auto posiciones = rabinKarpSearch(contenido, patron);
        if (!posiciones.empty()) {
            std::cout << "RK encontro en: " << ruta << " posiciones: ";
            for (int p : posiciones) std::cout << p << " ";
            std::cout << std::endl;
        }
    }

    return 0;
}