#ifndef TEST_UTILS_LIBRARY_H
#define TEST_UTILS_LIBRARY_H

#include "iostream"
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <queue>
#include <mutex>

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

    std::string readFile(const std::string &file);

    std::string getMessageStr(const std::string &data);

    /**
     * Apply formatting modification to the text and return the resulting test
     * @param text initial text
     * @param edits formatting operations
     * @return resulting text
     */
    std::string applyModifications(std::string text, std::vector<edit > edits);

    bool inRange(range a, range b);

    int positionInText(std::string text, position pos);

    /**
     * Simulate an stream to controlling the text provided to the server
     */
    class my_stringstreambuf : public std::basic_streambuf<char, std::char_traits<char>> {
    private:
        std::queue<int> q;
        std::mutex mut;
    public:
        explicit my_stringstreambuf(const std::string &initText) {
            std::lock_guard<std::mutex> lock(mut);
            for (auto c: initText) {
                q.push((int) c);
            }
        }

        void appendStr(const std::string &next) {
            std::lock_guard<std::mutex> lock(mut);
            for (auto c: next) {
                q.push(c);
            }
        }

        void setEof() {
            std::lock_guard<std::mutex> lock(mut);
            q.push(EOF);
        }

    protected:
        int uflow() override {
            while (currentChar() == -2) {}
            int c = nextChar();
            return c;
        }

        int currentChar() {
            std::lock_guard<std::mutex> lock(mut);
            return getChar();
        }

        int nextChar() {
            std::lock_guard<std::mutex> lock(mut);
            int c = getChar();
            q.pop();
            return c;
        }

    private:
        int getChar() {
            if (q.empty()) {
                return -2;
            }
            return (int) q.front();
        }
    };
}

#endif //TEST_UTILS_LIBRARY_H
