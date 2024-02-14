//
// Created by gonzalezext on 28.01.24.
// Updated by: stescobedo92 on 12.02.24
//

#include "utils.h"
#include <stdexcept>
#include <sstream>

std::string test_utils::extractJson(const std::string_view text, std::size_t jsonIt) {
    std::string res;
    std::size_t pos = 0;

    while (jsonIt--) {
        res.clear(); // Clear the string before use
        char startChar = findStartChar(text, pos);
        char endChar = (startChar == '{') ? '}' : ']';

        if (pos >= text.size()) {
            // json not found
            return "";
        }

        std::size_t count = 0;
        std::size_t initialCapacity = text.size() - pos; // Estimate initial capacity
        res.reserve(initialCapacity);
        do {
            res.push_back(text[pos]);
            if (text[pos] == startChar) {
                ++count;
            }
            if (text[pos] == endChar) {
                --count;
            }
            ++pos;
        } while (count > 0 && pos < text.size());

        if (count != 0) {
            // Malformed JSON
            throw std::runtime_error("Malformed JSON");
        }
    }
    return res;
}

std::string test_utils::applyModifications(const std::string_view text, std::vector<edit>& edits) {
    // validate ranges
    for (std::size_t i = 0; i < edits.size(); i++) {
        for (std::size_t j = i + 1; j < edits.size(); j++) {
            if (inRange(edits[i].first, edits[j].first) || inRange(edits[j].first, edits[i].first)) {
                std::cout << "Error with range into range" << std::endl;
                throw std::runtime_error("Error with range into range");
            }
        }
    }

    // Sort edits in descending order by range start
    std::sort(edits.begin(), edits.end(), [](const auto& a, const auto& b) {
        return a.first.first > b.first.first;
    });

    std::string res(text);
    for (const auto &ed: edits) {
        std::size_t start = positionInText(res, ed.first.first);
        std::size_t end = positionInText(res, ed.first.second);

        if (start > res.size() || end > res.size() || start > end) {
            throw std::runtime_error("Invalid edit range");
        }

        res.replace(start, end - start, ed.second);
    }

    return res;
}

bool test_utils::inRange(const range &a, const range &b) {
    return a.first <= b.first && a.second >= b.second;
}

std::size_t test_utils::positionInText(const std::string_view text, const position pos) {
    int line = 0, character = 0;
    for (int i = 0; i < text.size(); i++) {
        if (line < pos.first) {
            if (text[i] == '\n') {
                line++;
            }
        } else if (character < pos.second) {
            if (text[i] == '\n') {
                break;
            }
            character++;
        } else if (line == pos.first && character == pos.second) {
            return i;
        }
    }
    if (line == pos.first && character == pos.second) {
        return (int) text.size();
    }

    // If the position was not found, throw an exception
    std::ostringstream error_message;
    error_message << "Error with position (line: " << pos.first << ", character: " << pos.second << ")";
    throw std::runtime_error(error_message.str());
}

std::string test_utils::readFile(const std::string_view filePath, const bool prepareForSend) {
    std::ifstream file(filePath.data());
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    std::string fileContent;
    char c;
    while (file.get(c)) {
        if (prepareForSend) {
            if (c == '\"' || c == '\n') {
                fileContent.push_back('\\');
            }
            if (c == '\n') {
                fileContent.push_back('n');
                continue;
            }
            if (c == '\t') {
                fileContent.push_back('t');
                continue;
            }
        }
        fileContent.push_back(c);
    }
    file.close();

    return fileContent;
}

std::string test_utils::getMessageStr(const std::string_view data) {
    std::ostringstream headers_response;
    headers_response << "Content-Length: " << data.size() << "\r\n"
                     << "Content-Type: application/vscode; charset=utf-8\r\n\r\n"
                     << data;

    return headers_response.str();
}

uint64_t test_utils::getTimeNow() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
}

int test_utils::countJson(std::string_view text) {
    int c = 1;
    while (!extractJson(text, c).empty()) {
        c++;
    }
    return c - 1;
}
