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
    std::size_t line = 0, character = 0;
    for (const char c : text) {
        const bool is_newline = (c == '\n');

        // Increment the line if a line break is found and we are before the target position
        line += (line < pos.first && is_newline);

        // Increment character if it is not a line break and we are before the target position
        character += (!is_newline && character < pos.second);

        // If we reach the target position, return the current position
        if (line == pos.first && character == pos.second)
            return &c - text.data();
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
        fileContent.push_back((c == '\"' && prepareForSend) ? '\\' : c);
        if (prepareForSend && (c == '\n' || c == '\t')) {
            fileContent.push_back((c == '\n') ? 'n' : 't');
        }
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
