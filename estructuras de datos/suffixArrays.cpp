#include <vector>
#include <string>
#include <algorithm>

using vi = std::vector<int>;

// -----------------------------------------------------
// Construcción de suffix array (doubling + counting sort)
// -----------------------------------------------------
static void counting_sort(const vi& cls, vi& p) {
    int n = p.size();
    int classes = *std::max_element(cls.begin(), cls.end()) + 1;
    vi cnt(classes, 0), p_new(n), pos(classes);
    for (int c : cls) cnt[c]++;
    pos[0] = 0;
    for (int i = 1; i < classes; ++i) pos[i] = pos[i-1] + cnt[i-1];
    for (int x : p) {
        int c = cls[x];
        p_new[pos[c]++] = x;
    }
    p.swap(p_new);
}

std::vector<int> suffix_array_construction(const std::string& s_in) {
    // Trabajamos sobre s+'$'
    std::string s = s_in;
    s.push_back('$');
    int n = s.size();

    // k = 0: orden por carácter
    vi p(n), c(n), cnt(std::max(256, n), 0);
    for (unsigned char ch : s) cnt[ch]++;
    for (int i = 1; i < 256; ++i) cnt[i] += cnt[i-1];
    for (int i = 0; i < n; ++i)
        p[--cnt[(unsigned char)s[i]]] = i;

    c[p[0]] = 0;
    int classes = 1;
    for (int i = 1; i < n; ++i) {
        if (s[p[i]] != s[p[i-1]]) ++classes;
        c[p[i]] = classes - 1;
    }

    // Doubling
    vi p_new(n), c_new(n);
    for (int h = 0; (1 << h) < n; ++h) {
        for (int i = 0; i < n; ++i) {
            p_new[i] = p[i] - (1 << h);
            if (p_new[i] < 0) p_new[i] += n;
        }
        counting_sort(c, p_new);
        p = p_new;

        c_new[p[0]] = 0;
        int classes_new = 1;
        for (int i = 1; i < n; ++i) {
            int cur1 = c[p[i]], cur2 = c[(p[i] + (1 << h)) % n];
            int prev1 = c[p[i-1]], prev2 = c[(p[i-1] + (1 << h)) % n];
            if (cur1 != prev1 || cur2 != prev2) ++classes_new;
            c_new[p[i]] = classes_new - 1;
        }
        c = c_new;
        classes = classes_new;
    }

    // Eliminamos el sufijo que empieza en '$' (era p[0])
    p.erase(p.begin());
    return p;
}

// -----------------------------------------------------
// Búsqueda usando el suffix array
// -----------------------------------------------------
std::vector<int>
suffixArraySearch(const std::string& text_with_sentinel,
                  const std::string& pattern,
                  const std::vector<int>& SA) {
    int n = SA.size();
    int m = pattern.size();
    int l = 0, r = n;
    // lower bound
    while (l < r) {
        int mid = (l + r) / 2;
        if (text_with_sentinel.compare(SA[mid], m, pattern) < 0)
            l = mid + 1;
        else
            r = mid;
    }
    int start = l;
    l = 0; r = n;
    // upper bound
    while (l < r) {
        int mid = (l + r) / 2;
        if (text_with_sentinel.compare(SA[mid], m, pattern) <= 0)
            l = mid + 1;
        else
            r = mid;
    }
    std::vector<int> res;
    for (int i = start; i < l; ++i)
        res.push_back(SA[i]);
    return res;
}
