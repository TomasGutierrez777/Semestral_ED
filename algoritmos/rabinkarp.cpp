#include <string>               // para manipular cadenas
#include <vector>               // para almacenar resultados

std::vector<int> rabinKarpSearch(const std::string& text, const std::string& pattern) {
    const int d = 256;          // tamaño del alfabeto (bytes)
    const int q = 101;          // primo para el módulo del hash
    int m = pattern.size();     // longitud del patrón
    int n = text.size();        // longitud del texto
    std::vector<int> result;     // almacena índices donde hay match

    if (m > n) return result;    // patrón más largo que texto → sin resultados

    int h = 1;                   // factor para remover el carácter más antiguo
    for (int i = 0; i < m - 1; i++)
        h = (h * d) % q;         // calcula d^(m-1) mod q

    int p = 0, t = 0;            // hash del patrón (p) y de la ventana (t)

    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;  // acumula hash de pattern
        t = (d * t + text[i]) % q;     // acumula hash de la primera ventana
    }

    for (int i = 0; i <= n - m; i++) {
        if (p == t) {               // si hashes coinciden, posible match
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {  // confirmación carácter a carácter
                    match = false;
                    break;
                }
            }
            if (match)
                result.push_back(i);  // guardamos inicio del match
        }

        if (i < n - m) {
            // actualiza hash: remueve text[i], agrega text[i+m]
            t = (d * (t - text[i] * h) + text[i + m]) % q;
            if (t < 0) t += q;      // corrige negativo tras módulo
        }
    }

    return result;               // devolvemos todas las coincidencias
}
