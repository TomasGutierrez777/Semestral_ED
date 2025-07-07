// main.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <random>

#include "algoritmos/boyermoore.cpp"
#include "algoritmos/kmp.cpp"
#include "algoritmos/rabinkarp.cpp"
#include "estructuras de datos/suffixArrays.cpp"

namespace fs = std::filesystem;
using Clock = std::chrono::high_resolution_clock;
using ns    = std::chrono::nanoseconds;

// -------------- funciones auxiliares --------------

// Lee todos los .txt de la carpeta y devuelve pares (ruta, contenido)
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

// Genera N patrones aleatorios de longitudes entre minLen y maxLen,
// usando caracteres alfanuméricos [A–Z][a–z][0–9]
std::vector<std::string> generarPatronesAleatorios(int N,
                                                   int minLen = 3,
                                                   int maxLen = 7) {
    std::vector<std::string> patrones;
    patrones.reserve(N);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> lenDist(minLen, maxLen);
    // 0–61: 0–25 → 'A'–'Z'; 26–51 → 'a'–'z'; 52–61 → '0'–'9'
    std::uniform_int_distribution<int> charDist(0, 61);

    for (int i = 0; i < N; ++i) {
        int L = lenDist(gen);
        std::string p;
        p.reserve(L);
        for (int j = 0; j < L; ++j) {
            int x = charDist(gen);
            if (x < 26)          p.push_back(char('A' + x));
            else if (x < 52)     p.push_back(char('a' + (x - 26)));
            else /* 52–61 */     p.push_back(char('0' + (x - 52)));
        }
        patrones.push_back(std::move(p));
    }
    return patrones;
}

int main() {
    // 1) Cargo TODOS los documentos
    auto todos = leerArchivosRecursivos("documentos");
    int total = todos.size();
    if (!total) {
        std::cerr << "No hay archivos en 'documentos'\n";
        return 1;
    }

    // 2) Construyo T_full y offsets_full para **todo** el corpus
    std::string T_full;
    std::vector<int> offsets_full;
    size_t cap = 0;
    for (auto &p : todos) cap += p.second.size()+1;
    T_full.reserve(cap);
    offsets_full.reserve(total+1);
    for (auto &p : todos) {
        offsets_full.push_back(T_full.size());
        T_full += p.second;
        T_full.push_back('$');
    }
    offsets_full.push_back(T_full.size());

    // 3) Construyo el SA **solo una vez**
    std::cout << "-- Construyendo SA sobre " << total << " archivos --\n";
    auto t0 = Clock::now();
    auto sa_full = suffix_array_construction(T_full + "$");
    auto t1 = Clock::now();
    std::cout << "  Tiempo build SA: "
              << ns(t1 - t0).count()*1e-6
              << " ms\n";

    // 4) Configuro los experimentos:
    std::vector<int> lotesDoc, lotesPat = {100,200,300,1000,2000};
    for (int k = 10; k <= std::min(total,100); k+=10)
        lotesDoc.push_back(k);

    // 5) Para cada tamaño de corpus parcial...
    for (int Ndoc : lotesDoc) {
        std::cout << "\n=== Usando primeros " << Ndoc << " archivos ===\n";
        int limite = offsets_full[Ndoc];

        // 6) Para cada lote de patrones...
        for (int Npat : lotesPat) {
            auto patrones = generarPatronesAleatorios(Npat,3,7);
            double tbm=0, tk=0, tr=0, tsa=0;

            for (auto &p : patrones) {
                auto a = Clock::now();
                boyerMooreSearch(T_full, p);
                tbm += ns(Clock::now()-a).count()*1e-6;

                auto b = Clock::now();
                kmpSearch(T_full, p);
                tk += ns(Clock::now()-b).count()*1e-6;

                auto c = Clock::now();
                rabinKarpSearch(T_full, p);
                tr += ns(Clock::now()-c).count()*1e-6;

                auto d = Clock::now();
                // buscas en todo T_full pero filtras después
                auto occ = suffixArraySearch(T_full+"$", p, sa_full);
                tsa += ns(Clock::now()-d).count()*1e-6;
                // opcional: filtrar occ < limite si quieres contar/exhibir posiciones
            }

            std::cout << "  -- " << Npat << " patrones --\n"
                      << "    BM:  " << tbm << " ms\n"
                      << "    KMP: " << tk  << " ms\n"
                      << "    RK:  " << tr  << " ms\n"
                      << "    SA:  " << tsa << " ms\n";
        }
    }
    return 0;
}