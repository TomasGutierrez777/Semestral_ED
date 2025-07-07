#include <vector>                          // para el resultado de posiciones
#include <string>                          // manejo de cadenas de texto
#include <unordered_map>                   // tabla badChar para malos caracteres

std::vector<int> boyerMooreSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> result;               // guarda índices de ocurrencia
    int n = text.size(), m = pattern.size();  
    if (m == 0) return result;             // patrón vacío no busca

    std::unordered_map<char, int> badChar;  
    for (int i = 0; i < m; i++)            
        badChar[pattern[i]] = i;           // badChar guarda última posición de cada carácter en pattern

    int shift = 0;                         // desplazamiento actual en text
    while (shift <= n - m) {               // mientras pattern entre en el fragmento restante
        int j = m - 1;                     // empezamos comparando desde el final
        while (j >= 0 && pattern[j] == text[shift + j])
            j--;                          // retrocede mientras coincidan
        if (j < 0) {                       
            result.push_back(shift);      // coincidencia completa en shift
            shift += (shift + m < n)      
                     ? m - badChar[text[shift + m]]  // salto según siguiente carácter en text
                     : 1;                // si no hay siguiente carácter, avanzamos 1
        } else {                           
            shift += std::max(1, j - (badChar.count(text[shift + j]) ? badChar[text[shift + j]] : -1));  // salto tras desajuste
        }
    }

    return result;                        
}
