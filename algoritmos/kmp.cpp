#include <vector>
#include <string>

std::vector<int> kmpSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> result;
    std::vector<int> lps(pattern.size(), 0);
    int len = 0;

    for (int i = 1; i < pattern.size(); ) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        } else {
            if (len != 0)
                len = lps[len - 1];
            else
                lps[i++] = 0;
        }
    }

    int i = 0, j = 0;
    while (i < text.size()) {
        if (text[i] == pattern[j]) {
            i++; j++;
        }
        if (j == pattern.size()) {
            result.push_back(i - j);
            j = lps[j - 1];
        } else if (i < text.size() && text[i] != pattern[j]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }

    return result;
}