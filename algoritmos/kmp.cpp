#include <vector>                       // para guardar resultados
#include <string>                       // manejo de cadenas

std::vector<int> kmpSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> result;             // almacena índices de coincidencia
    std::vector<int> lps(pattern.size(), 0);  // tabla de prefijo-sufijo
    int len = 0;                         // longitud actual de prefijo-sufijo

    // Construcción de lps
    for (int i = 1; i < pattern.size(); ) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;           // extiende prefijo-sufijo
        } else {
            if (len != 0)
                len = lps[len - 1];    // retrocede len al último valor válido
            else
                lps[i++] = 0;           // no hay prefijo que sea sufijo
        }
    }

    int i = 0, j = 0;                    // i → índice en text, j → índice en pattern
    while (i < text.size()) {
        if (text[i] == pattern[j]) {
            i++; j++;                   // coincidencia parcial
        }
        if (j == pattern.size()) {
            result.push_back(i - j);   // patrón completo encontrado
            j = lps[j - 1];            // salto según lps para siguiente búsqueda
        } else if (i < text.size() && text[i] != pattern[j]) {
            if (j != 0)
                j = lps[j - 1];        // usa lps para retroceder j
            else
                i++;                  // avanza i si j=0
        }
    }

    return result;                     // devuelve todas las posiciones
}
