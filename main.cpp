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
#include "estructuras de datos/suffixArrays.cpp"

namespace fs = std::filesystem;

using Clock = std::chrono::high_resolution_clock;
using ns    = std::chrono::nanoseconds;

// Lee recursivamente todos los .txt de la carpeta y devuelve pares (ruta, contenido)
std::vector<std::pair<std::string, std::string>>
leerArchivosRecursivos(const std::string& carpeta) {
    std::vector<std::pair<std::string, std::string>> archivos;
    for (auto const& entry : fs::recursive_directory_iterator(carpeta)) {
        if (!entry.is_regular_file()) continue;
        std::ifstream in(entry.path());
        if (!in.is_open()) continue;
        std::string contenido, linea;
        while (std::getline(in, linea)) {
            contenido += linea + "\n";
        }
        archivos.emplace_back(entry.path().string(), contenido);
    }
    return archivos;
}

// Mapea posiciones globales en T_full a posiciones por documento
std::vector<std::vector<int>> mapearPosiciones(
    const std::vector<int>& posiciones,
    const std::vector<int>& offsets
) {
    int n_docs = int(offsets.size()) - 1;
    std::vector<std::vector<int>> resultados(n_docs);
    for (int pos : posiciones) {
        auto it = std::upper_bound(offsets.begin(), offsets.end(), pos);
        if (it == offsets.begin() || it == offsets.end()) continue;
        int idx = int(it - offsets.begin()) - 1;
        resultados[idx].push_back(pos - offsets[idx]);
    }
    return resultados;
}

int main() {
    const std::string carpeta = "documentos";
    std::cout << "Ingrese patron a buscar: ";
    std::string patron;
    std::getline(std::cin, patron);

    // 1) Cargo TODOS los documentos
    auto todos = leerArchivosRecursivos(carpeta);
    int total = (int)todos.size();
    if (total == 0) {
        std::cerr << "No se encontraron archivos en '" << carpeta << "'\n";
        return 1;
    }

    // 2) Concatenar T_full y offsets_full
    std::string T_full;
    std::vector<int> offsets_full;
    size_t suma = 0;
    for (auto &p : todos) suma += p.second.size() + 1;
    T_full.reserve(suma);
    offsets_full.reserve(total + 1);
    for (auto &p : todos) {
        offsets_full.push_back((int)T_full.size());
        T_full += p.second;
        T_full.push_back('$');
    }
    offsets_full.push_back((int)T_full.size());

    // 3) Construir SA_FULL UNA SOLA VEZ (sobre T_full + '$')
    std::cout << "\n-- SA_FULL build --\n";
    auto t_sa0 = Clock::now();
    auto sa_full = suffix_array_construction(T_full + "$");
    auto t_sa1 = Clock::now();
    std::cout << "Tiempo build SA: "
              << ns(t_sa1 - t_sa0).count() * 1e-6
              << " ms\n";

    // 4) Tamaños de experimento: 10,20,... hasta total
    std::vector<int> tamaños;
    for (int k = 10; k <= total; k += 10) tamaños.push_back(k);
    if (tamaños.empty()) tamaños.push_back(total);

    // Lambda de impresión
    auto imprimir = [&](const std::string& nombre,
                        const std::vector<std::vector<int>>& res,
                        double tiempo_ms,
                        const std::vector<std::pair<std::string,std::string>>& docs) {
        std::cout << "\n-- " << nombre
                  << " (" << tiempo_ms << " ms) --\n";
        for (int i = 0; i < (int)res.size(); ++i) {
            if (!res[i].empty()) {
                std::cout << "  " << docs[i].first << ": ";
                for (int p : res[i]) std::cout << p << " ";
                std::cout << "\n";
            }
        }
    };

    // 5) Bucle de experimentos
    for (int k : tamaños) {
        std::cout << "\n=== Experimento con " << k << " documentos ===\n";
        auto docs = std::vector<std::pair<std::string,std::string>>(
            todos.begin(), todos.begin() + k
        );
        int limite = offsets_full[k];

        // Boyer-Moore
        auto t0 = Clock::now();
        auto bm_full = boyerMooreSearch(T_full, patron);
        auto t1 = Clock::now();
        double bm_ms = ns(t1 - t0).count() * 1e-6;
        std::vector<int> bm_pos;
        for (int p : bm_full) if (p < limite) bm_pos.push_back(p);
        auto bm_res = mapearPosiciones(bm_pos,
            std::vector<int>(offsets_full.begin(),
                             offsets_full.begin() + k + 1)
        );
        imprimir("Boyer-Moore", bm_res, bm_ms, docs);

        // KMP
        auto t2 = Clock::now();
        auto kmp_full = kmpSearch(T_full, patron);
        auto t3 = Clock::now();
        double kmp_ms = ns(t3 - t2).count() * 1e-6;
        std::vector<int> kmp_pos;
        for (int p : kmp_full) if (p < limite) kmp_pos.push_back(p);
        auto kmp_res = mapearPosiciones(kmp_pos,
            std::vector<int>(offsets_full.begin(),
                             offsets_full.begin() + k + 1)
        );
        imprimir("Knuth-Morris-Pratt", kmp_res, kmp_ms, docs);

        // Rabin-Karp
        auto t4 = Clock::now();
        auto rk_full = rabinKarpSearch(T_full, patron);
        auto t5 = Clock::now();
        double rk_ms = ns(t5 - t4).count() * 1e-6;
        std::vector<int> rk_pos;
        for (int p : rk_full) if (p < limite) rk_pos.push_back(p);
        auto rk_res = mapearPosiciones(rk_pos,
            std::vector<int>(offsets_full.begin(),
                             offsets_full.begin() + k + 1)
        );
        imprimir("Rabin-Karp", rk_res, rk_ms, docs);

        // Suffix-Array search (uso T_full+"$")
        auto t6 = Clock::now();
        auto sa_full_pos = suffixArraySearch(T_full + "$", patron, sa_full);
        auto t7 = Clock::now();
        double sa_ms = ns(t7 - t6).count() * 1e-6;
        std::vector<int> sa_pos;
        for (int p : sa_full_pos) if (p < limite) sa_pos.push_back(p);
        auto sa_res = mapearPosiciones(sa_pos,
            std::vector<int>(offsets_full.begin(),
                             offsets_full.begin() + k + 1)
        );
        imprimir("Suffix-Array search", sa_res, sa_ms, docs);
    }

    return 0;
}