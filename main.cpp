#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <chrono>

#include "algoritmos/boyermoore.cpp"
#include "algoritmos/kmp.cpp"
#include "algoritmos/rabinkarp.cpp"

namespace fs = std::filesystem;

using Clock = std::chrono::high_resolution_clock;
using ns    = std::chrono::nanoseconds;

std::vector<std::pair<std::string, std::string>>
leerArchivosRecursivos(const std::string& carpeta) {
    std::vector<std::pair<std::string, std::string>> archivos;
    for (const auto& entry : fs::recursive_directory_iterator(carpeta)) {
        if (!entry.is_regular_file()) continue;
        std::ifstream archivo(entry.path());
        if (!archivo.is_open()) continue;

        std::string contenido, linea;
        while (std::getline(archivo, linea)) {
            contenido += linea + "\n";
        }
        archivos.emplace_back(entry.path().string(), contenido);
    }
    return archivos;
}

std::vector<std::vector<int>> mapearPosiciones(
    const std::vector<int>& posiciones,
    const std::vector<int>& offsets,
    int n_docs
) {
    std::vector<std::vector<int>> resultados(n_docs);
    for (int pos : posiciones) {
        auto it = std::upper_bound(offsets.begin(), offsets.end(), pos);
        int idx = int(it - offsets.begin()) - 1;
        if (0 <= idx && idx < n_docs) {
            int relativo = pos - offsets[idx];
            resultados[idx].push_back(relativo);
        }
    }
    return resultados;
}

int main() {
    const std::string carpeta = "documentos";
    std::string patron;
    std::cout << "Ingrese patron a buscar: ";
    std::getline(std::cin, patron);

    auto archivos = leerArchivosRecursivos(carpeta);
    if (archivos.empty()) {
        std::cerr << "No se encontraron archivos en '" << carpeta << "'.\n";
        return 1;
    }

    std::string T;
    std::vector<int> offsets;
    for (auto& [ruta, contenido] : archivos) {
        offsets.push_back((int)T.size());
        T += contenido;
        T += '$';
    }
    offsets.push_back((int)T.size());
    int n = (int)archivos.size();

    auto t0 = Clock::now();
    auto bm_pos = boyerMooreSearch(T, patron);
    auto t1 = Clock::now();
    double bm_ms = ns(t1 - t0).count() * 1e-6;

    auto t2 = Clock::now();
    auto kmp_pos = kmpSearch(T, patron);
    auto t3 = Clock::now();
    double kmp_ms = ns(t3 - t2).count() * 1e-6;

    auto t4 = Clock::now();
    auto rk_pos = rabinKarpSearch(T, patron);
    auto t5 = Clock::now();
    double rk_ms = ns(t5 - t4).count() * 1e-6;

    auto bm_res  = mapearPosiciones(bm_pos,  offsets, n);
    auto kmp_res = mapearPosiciones(kmp_pos, offsets, n);
    auto rk_res  = mapearPosiciones(rk_pos,  offsets, n);

    auto imprimir = [&](const std::string& nombre,
                        const std::vector<std::vector<int>>& res,
                        double tiempo_ms){
        std::cout << "\n=== " << nombre
                  << " (" << tiempo_ms << " ms) ===\n";
        for (int i = 0; i < n; ++i) {
            if (!res[i].empty()) {
                std::cout << archivos[i].first << " posiciones: ";
                for (int p : res[i]) std::cout << p << " ";
                std::cout << "\n";
            }
        }
    };

    imprimir("Boyer-Moore", bm_res, bm_ms);
    imprimir("Knuth-Morris-Pratt", kmp_res, kmp_ms);
    imprimir("Rabin-Karp", rk_res, rk_ms);

    return 0;
}
