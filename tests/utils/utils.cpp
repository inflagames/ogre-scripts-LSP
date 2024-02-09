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

std::string test_utils::applyModifications(std::string text, std::vector<edit > edits) {
    std::string res = std::move(text);

    // validate ranges
    for (int i = 0; i < edits.size(); i++) {
        for (int j = i + 1; j < edits.size(); j++) {
            if (inRange(edits[i].first, edits[j].first) || inRange(edits[j].first, edits[i].first)) {
                std::cout << "Error with range into range" << std::endl;
                throw std::exception();
            }
        }
    }

    std::sort(edits.begin(), edits.end());
    std::reverse(edits.begin(), edits.end());

    for (const auto &ed: edits) {
        int start = positionInText(res, ed.first.first);
        int end = positionInText(res, ed.first.second);
        res = res.substr(0, start) + ed.second + res.substr(end);
    }

    return res;
}

bool test_utils::inRange(range a,
                         range b) {
    return a.first <= b.first && a.second >= b.second;
}

int test_utils::positionInText(std::string text, position pos) {
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
    std::cout << "Error with position ( line:" << pos.first << ", character: " << pos.second << ")" << std::endl;
    throw std::exception();
}

std::string test_utils::readFile(const std::string &filePath) {
    std::ifstream file;
    std::string fileContent;
    file.open(filePath);
    if (!file.is_open()) {
        throw std::exception();
    }
    while (!file.eof()) {
        char c;
        file.get(c);
        if (file.eof()) {
            break;
        }
        fileContent.push_back(c);
    }
    file.close();
    return fileContent;
}

std::string test_utils::getMessageStr(const std::string &data) {
    return "Content-Length: " + std::to_string(data.size()) +
           "\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n" + data;
}

uint64_t test_utils::getTimeNow() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}
