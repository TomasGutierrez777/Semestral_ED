#include <vector>
#include <string>
#include <unordered_map>

std::vector<int> boyerMooreSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> result;
    int n = text.size(), m = pattern.size();
    if (m == 0) return result;

    std::unordered_map<char, int> badChar;
    for (int i = 0; i < m; i++)
        badChar[pattern[i]] = i;

    int shift = 0;
    while (shift <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[shift + j])
            j--;
        if (j < 0) {
            result.push_back(shift);
            shift += (shift + m < n) ? m - badChar[text[shift + m]] : 1;
        } else {
            shift += std::max(1, j - badChar[text[shift + j]]);
        }
    }

    return result;
}