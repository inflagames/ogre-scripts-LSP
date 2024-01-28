#ifndef TEST_UTILS_LIBRARY_H
#define TEST_UTILS_LIBRARY_H

#include <string>

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
};

#endif //TEST_UTILS_LIBRARY_H
