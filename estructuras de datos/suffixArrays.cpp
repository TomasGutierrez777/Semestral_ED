#include <vector>
#include <string>
#include <iostream>

//Codigo extraido de: https://cp-algorithms.com/string/suffix-array.html

//Suffix Array: Doubling
std::vector<int> sort_cyclic_shifts(std::string const& s) {
    int n = s.size();
    const int alphabet = 256;

std::vector<int> p(n), c(n), cnt(std::max(alphabet, n), 0);
    for (int i = 0; i < n; i++)
        cnt[s[i]]++;
    for (int i = 1; i < alphabet; i++)
        cnt[i] += cnt[i-1];
    for (int i = 0; i < n; i++)
        p[--cnt[s[i]]] = i;
    c[p[0]] = 0;
    int classes = 1;
    for (int i = 1; i < n; i++) {
        if (s[p[i]] != s[p[i-1]])
            classes++;
        c[p[i]] = classes - 1;
    }

std::vector<int> pn(n), cn(n);
    for (int h = 0; (1 << h) < n; ++h) {
        for (int i = 0; i < n; i++) {
            pn[i] = p[i] - (1 << h);
            if (pn[i] < 0)
                pn[i] += n;
        }
        fill(cnt.begin(), cnt.begin() + classes, 0);
        for (int i = 0; i < n; i++) 
            cnt[c[pn[i]]]++;
        for (int i = 1; i < classes; i++) 
            cnt[i] += cnt[i-1];
        for (int i = n-1; i >= 0; i--) 
            p[--cnt[c[pn[i]]]] = pn[i];
        cn[p[0]] = 0;
        classes = 1;

        for (int i = 1; i < n; i++) {
            std::pair<int, int> cur = {c[p[i]], c[(p[i] + (1 << h)) % n]};
            std::pair<int, int> prev = {c[p[i-1]], c[(p[i-1] + (1 << h)) % n]};
            if (cur != prev)
                ++classes;
            cn[p[i]] = classes - 1;
        }
        c.swap(cn);
    }
    return p;
}

std::vector<int> suffix_array_construction(std::string s) {
    s += "$";
    std::vector<int> sorted_shifts = sort_cyclic_shifts(s);
    sorted_shifts.erase(sorted_shifts.begin());
    return sorted_shifts;
}

//Busqueda de patron usando SA 
std::vector<int> suffixArraySearch(const std::string& text, const std::string& pattern) {
    std::cout << "Inicia la construcción de la estructura..." << std::endl;
    auto SA = suffix_array_construction(text);
    std::cout << "Termina la construcción." << std::endl;

    std::vector<int> result;

    int n = text.size();
    int m = pattern.size();

    int left = 0, right = n;

    while (left < right) {
        int mid = (left + right) / 2;
        int pos = SA[mid];
        std::string suffix = text.substr(pos, m);
        if (suffix < pattern) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    int start = left;
    right = n;

    while (left < right) {
        int mid = (left + right) / 2;
        int pos = SA[mid];
        std::string suffix = text.substr(pos, m);
        if (suffix <= pattern) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    int end = left;

    for (int i = start; i < end; ++i) {
        result.push_back(SA[i]);
    }

    return result;
}