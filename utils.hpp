// utils.hpp
#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdint>

// Read entire file into string
inline std::string read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return content;
}

// Write string to file
inline void write_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot write file: " + filename);
    }
    file << content;
}

// Split string by whitespace
inline std::vector<std::string> split_on_whitespace(const std::string& text) {
    std::vector<std::string> words;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}

// Convert byte to string (e.g., for single-byte symbols)
inline std::string byte_to_string(uint8_t byte) {
    return std::string(1, static_cast<char>(byte));
}

// Is valid UTF-8 continuation byte?
inline bool is_utf8_continuation(uint8_t c) {
    return (c & 0xC0) == 0x80;
}

// Validate UTF-8 (basic check)
inline bool is_valid_utf8(const std::string& str) {
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(str.data());
    size_t i = 0;
    while (i < str.size()) {
        if (bytes[i] < 0x80) {
            i += 1;
        } else if ((bytes[i] & 0xE0) == 0xC0) {
            if (i+1 >= str.size() || !is_utf8_continuation(bytes[i+1])) return false;
            i += 2;
        } else if ((bytes[i] & 0xF0) == 0xE0) {
            if (i+2 >= str.size() || !is_utf8_continuation(bytes[i+1]) || !is_utf8_continuation(bytes[i+2])) return false;
            i += 3;
        } else if ((bytes[i] & 0xF8) == 0xF0) {
            if (i+3 >= str.size() || !is_utf8_continuation(bytes[i+1]) || !is_utf8_continuation(bytes[i+2]) || !is_utf8_continuation(bytes[i+3])) return false;
            i += 4;
        } else {
            return false;
        }
    }
    return true;
}