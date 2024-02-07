#ifndef TEST_UTILS_LIBRARY_H
#define TEST_UTILS_LIBRARY_H

#include "iostream"
#include <string>
#include <vector>
#include <fstream>
#include <utility>

#define position std::pair<int, int>
#define range std::pair<position, position>
#define edit std::pair<range, std::string>

/**
 * Only for testing support
 */
namespace test_utils {
    /**
     * Given the string search for the first valid json object on it starting on the position given.
     * @param text string to extract the json object
     * @param jsonIt number of json to return
     * @return return the sub-string with the json object
     */
    std::string extractJson(std::string text, int jsonIt = 1);

    std::string readFile(const std::string& file);

    std::string getMessageStr(const std::string& data);

    /**
     * Apply formatting modification to the text and return the resulting test
     * @param text initial text
     * @param edits formatting operations
     * @return resulting text
     */
    std::string applyModifications(std::string text, std::vector<edit> edits);

    bool inRange(range a, range b);

    int positionInText(std::string text, position pos);
};

#endif //TEST_UTILS_LIBRARY_H
