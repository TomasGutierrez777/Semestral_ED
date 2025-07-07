#pragma once
#include <vector>
#include <string>
#include <algorithm>

// --- Suffix Array: doubling + radix (two-pass counting sort) ---

static inline void counting_sort_pairs(std::vector<int>& p,
                                       const std::vector<int>& c,
                                       int classes) {
    int n = p.size();
    std::vector<int> cnt(classes, 0), pn(n), pos(classes);
    // 1) contar frecuencia de cada “clase”
    for (int x : c) cnt[x]++;
    // 2) prefijos para buckets
    pos[0] = 0;
    for (int i = 1; i < classes; ++i)
        pos[i] = pos[i-1] + cnt[i-1];
    // 3) ordenar p por clase c[p[i]]
    for (int x : p) {
        int cl = c[x];
        pn[ pos[cl]++ ] = x;
    }
    p.swap(pn);
}

std::vector<int> suffix_array_construction(std::string s) {
    s.push_back('$');
    int n = s.size();
    const int ALPHA = 256;

    // 1) k = 0: ordenamos por el carácter único
    std::vector<int> p(n), c(n), cnt(std::max(ALPHA, n), 0);
    for (char ch : s) cnt[(unsigned char)ch]++;
    for (int i = 1; i < ALPHA; ++i) cnt[i] += cnt[i-1];
    for (int i = 0; i < n; ++i)
        p[ --cnt[(unsigned char)s[i]] ] = i;

    c[p[0]] = 0;
    int classes = 1;
    for (int i = 1; i < n; ++i) {
        if (s[p[i]] != s[p[i-1]]) ++classes;
        c[p[i]] = classes - 1;
    }

    // 2) k -> k+1: duplicamos la longitud de comparación
    std::vector<int> pn(n), cn(n);
    for (int h = 0; (1 << h) < n; ++h) {
        // desplaza p[i] atrás 2^h para orden circular
        for (int i = 0; i < n; ++i) {
            pn[i] = p[i] - (1 << h);
            if (pn[i] < 0) pn[i] += n;
        }
        // radix sort: primero por segundo par, luego por primer par
        // pero como hemos desplazado, basta un counting sort por clase
        counting_sort_pairs(pn, c, classes);

        // reconstruir clases cn
        cn[p[0]] = 0;
        classes = 1;
        for (int i = 1; i < n; ++i) {
            int curr1 = c[p[i]];
            int curr2 = c[(p[i] + (1 << h)) % n];
            int prev1 = c[p[i-1]];
            int prev2 = c[(p[i-1] + (1 << h)) % n];
            if (curr1 != prev1 || curr2 != prev2) 
                ++classes;
            cn[p[i]] = classes - 1;
        }
        c.swap(cn);
    }

    // p[0] es la posición del ‘$’; lo quitamos para devolver SA
    p.erase(p.begin());
    return p;
}

// Búsqueda con Suffix Array sin cambios
inline std::vector<int> suffixArraySearch(const std::string& text,
                                          const std::string& pattern,
                                          const std::vector<int>& SA) {
    std::vector<int> result;
    int n = (int)text.size(), m = (int)pattern.size();
    int l = 0, r = n;
    while (l < r) {
        int mid = (l + r) / 2;
        if (text.compare(SA[mid], m, pattern) < 0) l = mid + 1;
        else r = mid;
    }
    int start = l; l = 0; r = n;
    while (l < r) {
        int mid = (l + r) / 2;
        if (text.compare(SA[mid], m, pattern) <= 0) l = mid + 1;
        else r = mid;
    }
    for (int i = start; i < l; ++i)
        result.push_back(SA[i]);
    return result;
}
