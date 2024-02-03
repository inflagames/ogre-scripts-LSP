//
// Created by gonzalezext on 28.01.24.
//

#include "utils.h"

std::string test_utils::extractJson(std::string text, int jsonIt) {
    std::string res;
    int pos = 0;
    while (jsonIt--) {
        res = "";
        char ii = '{', ff = '}';
        while (text[pos] != '{' && text[pos] != '[') pos++;
        if (text[pos] == '[') {
            ii = '[';
            ff = ']';
        }
        int c = 0;
        do {
            res.push_back(text[pos]);
            if (text[pos] == ii) c++;
            if (text[pos] == ff) c--;
            pos++;
        } while (c);
//        std::cout << res << std::endl;
    }
    return res;
}

//std::string test_utils::applyModifications(std::string text, std::vector<Range> ranges) {
//
//}
