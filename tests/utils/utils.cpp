//
// Created by gonzalezext on 28.01.24.
// Updated by: stescobedo92 on 12.02.24
//

#include "utils.h"

/**
 * @brief Extracts a JSON object or array from the provided text at the specified position.
 *
 * This function extracts a JSON object or array from the given text starting at the specified position. It iterates
 * through the text to locate the JSON structure indicated by curly braces '{' and '}' for objects or square brackets '['
 * and ']' for arrays. It then extracts the JSON content until the corresponding closing brace '}' or bracket ']' is found.
 *
 * @param text The text from which to extract the JSON object or array.
 * @param jsonIt The index indicating the position of the JSON object or array to extract.
 *
 * @return A string containing the extracted JSON object or array.
 *
 * @remarks
 *  - This function assumes that the provided text contains one or more JSON objects or arrays separated by commas ','.
 *  - It starts searching for the specified JSON structure from the beginning of the text and iterates until the
 *    specified JSON object or array is found.
 *  - The extracted JSON content, including the opening and closing braces or brackets, is returned as a string.
 *
 * @dependencies
 *  - None
 *
 * @note
 *  - Developers should ensure that the input text contains valid JSON structures and that the specified position
 *    accurately indicates the desired JSON object or array to extract.
 *  - This function is designed for extracting JSON content from text for parsing or processing in applications
 *    involving JSON-based data interchange.
 *  - It is recommended to handle edge cases such as invalid JSON structures or out-of-bounds positions to provide
 *    robust error handling.
 */
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

/**
 * @brief Applies a series of text modifications to the provided text string.
 *
 * This function applies a series of text modifications, specified by a vector of edit objects, to the given text
 * string. Each edit object contains a range indicating the portion of the text to be replaced and the new text
 * to replace it with. The function validates the ranges to ensure they do not overlap and then applies the edits
 * in reverse order to avoid shifting positions.
 *
 * @param text The original text string to which modifications will be applied.
 * @param edits A vector of edit objects representing the modifications to be applied.
 *
 * @return A string containing the modified text after applying all specified edits.
 *
 * @remarks
 *  - This function validates the ranges of edits to ensure that they do not overlap, which could lead to unexpected
 *    behavior or incorrect results.
 *  - Edits are applied in reverse order to prevent positional shifts caused by modifications affecting subsequent
 *    edit ranges.
 *  - The function uses a simple algorithm to locate the start and end positions of each edit range within the text
 *    string and then performs the replacement accordingly.
 *
 * @dependencies
 *  - This function relies on the `edit` structure for representing text modifications and the `positionInText` function
 *    for locating positions within the text string.
 *  - It also utilizes standard library functions for sorting the edits vector and manipulating substrings of the text.
 *
 * @note
 *  - Developers should ensure that the provided text and edit objects accurately represent the modifications to be
 *    applied and the ranges to be replaced.
 *  - It is recommended to handle potential exceptions or errors that may occur during text modification or range
 *    validation to provide robust error handling mechanisms.
 */
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

/**
 * @brief Checks if one range is completely contained within another range.
 *
 * This function determines whether one range (denoted by its start and end positions) is entirely contained within
 * another range. It returns true if the start of range 'b' is greater than or equal to the start of range 'a' and
 * the end of range 'b' is less than or equal to the end of range 'a'.
 *
 * @param a The first range to be compared.
 * @param b The second range to be compared.
 *
 * @return true if range 'b' is entirely contained within range 'a', false otherwise.
 *
 * @remarks
 *  - The function assumes that the range boundaries are inclusive.
 *  - It checks whether the start of range 'b' is greater than or equal to the start of range 'a' and whether
 *    the end of range 'b' is less than or equal to the end of range 'a'.
 *  - If both conditions are satisfied, it indicates that range 'b' is completely contained within range 'a',
 *    and the function returns true.
 *
 * @dependencies
 *  - None
 *
 * @note
 *  - Developers should ensure that the provided ranges are properly defined and represent valid ranges with
 *    appropriate start and end positions.
 *  - This function is useful for checking range containment relationships, such as when verifying overlapping
 *    or nested ranges.
 *  - It assumes that the boundaries of the ranges are inclusive, meaning that the positions denoted by the
 *    start and end values are included in the range.
 */
bool test_utils::inRange(range a, range b) {
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

/**
 * @brief Determines the position of a character within the provided text string.
 *
 * This function calculates the position of a character within the given text string based on its line and character
 * indices. It iterates through the text to locate the specified position, counting both lines and characters until
 * the desired position is reached.
 *
 * @param text The text string in which to locate the position.
 * @param pos A pair representing the line and character indices of the position to find.
 *
 * @return The index of the specified position within the text string.
 *
 * @remarks
 *  - This function interprets line and character indices starting from 0.
 *  - It iterates through the text, counting both lines and characters until the specified position (line and character)
 *    is reached.
 *  - The function returns the index of the specified position within the text string, representing the position at which
 *    the character is found.
 *
 * @dependencies
 *  - None
 *
 * @note
 *  - Developers should ensure that the provided text string and position indices accurately represent the location
 *    being searched for.
 *  - This function is useful for determining the exact position of characters within text strings, particularly in
 *    scenarios where specific locations need to be identified.
 *  - It is recommended to handle potential exceptions or errors that may occur during position calculation to provide
 *    robust error handling mechanisms.
 */
std::string test_utils::readFile(const std::string &filePath, bool prepareForSend) {
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

/**
 * @brief Constructs a message string with content length and type headers.
 *
 * This function constructs a message string with content length and type headers based on the provided data.
 * It formats the message string according to the Language Server Protocol (LSP) specification, including
 * the "Content-Length" and "Content-Type" headers followed by the actual data.
 *
 * @param data The data to be included in the message string.
 *
 * @return A string representing the formatted message with headers and data.
 *
 * @remarks
 *  - This function adheres to the LSP message format, which requires the "Content-Length" and "Content-Type" headers
 *    to be included in each message.
 *  - It calculates the size of the data string and includes it in the "Content-Length" header.
 *  - The "Content-Type" header specifies the type of content, typically set to "application/vscode" for LSP messages.
 *
 * @dependencies
 *  - None
 *
 * @note
 *  - Developers should ensure that the provided data string adheres to the LSP message content requirements.
 *  - This function is intended for use in constructing LSP messages for communication between language servers
 *    and clients in language processing applications.
 *  - It is recommended to handle potential errors or exceptions that may occur during message construction to
 *    provide robust error handling mechanisms.
 */
std::string test_utils::getMessageStr(const std::string &data) {
    return "Content-Length: " + std::to_string(data.size()) +
           "\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n" + data;
}

/**
 * @brief Obtains the current time in milliseconds since the epoch.
 *
 * This function retrieves the current time in milliseconds since the epoch (January 1, 1970, UTC). It utilizes
 * the C++ `<chrono>` library to obtain the current system time and convert it into milliseconds.
 *
 * @return The current time in milliseconds since the epoch.
 *
 * @remarks
 *  - The function returns the time elapsed since the epoch in milliseconds, providing a high-resolution timestamp
 *    for precise time measurements.
 *  - It utilizes the `std::chrono::system_clock::now()` function to retrieve the current system time.
 *  - The obtained time is then converted into milliseconds using `std::chrono::duration_cast`.
 *
 * @dependencies
 *  - This function depends on the `<chrono>` library for time-related operations and `std::chrono::system_clock`
 *    for retrieving the current system time.
 *
 * @note
 *  - Developers should be aware that the timestamp returned by this function may vary depending on the system's clock
 *    accuracy and resolution.
 *  - It is recommended to use this function for measuring time intervals or obtaining timestamps in applications
 *    requiring precise timing information.
 */
uint64_t test_utils::getTimeNow() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}
