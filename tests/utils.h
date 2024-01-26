//
// Created by guille on 27.01.24.
//

#include <string>

/**
 * Only for testing support
 */
namespace test_utils {
    /**
     * Given the string search for the first valid json object on it starting on the position given.
     * @param text string to extract the json object
     * @param pos position to start the search
     * @return return the sub-string with the json object
     */
    std::string extractJson(std::string text, int pos = 0) {
        std::string res;
        while (text[pos] != '{') pos++;
        int c = 0;
        do {
            res.push_back(text[pos]);
            if (text[pos] == '{') c++;
            if (text[pos] == '}') c--;
            pos++;
        } while (c);
        return res;
    }
};
