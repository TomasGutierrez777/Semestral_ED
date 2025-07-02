#include <string>
#include <vector>

std::vector<int> rabinKarpSearch(const std::string& text, const std::string& pattern) {
    const int d = 256;
    const int q = 101;
    int m = pattern.size();
    int n = text.size();
    std::vector<int> result;

    if (m > n) return result;

    int h = 1;
    for (int i = 0; i < m - 1; i++)
        h = (h * d) % q;

    int p = 0, t = 0;

    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }

    for (int i = 0; i <= n - m; i++) {
        if (p == t) {
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match)
                result.push_back(i);
        }

        if (i < n - m) {
            t = (d * (t - text[i] * h) + text[i + m]) % q;
            if (t < 0) t += q;
        }
    }

    return result;
}
