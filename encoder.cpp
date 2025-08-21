#include "bpe.hpp"
#include "utils.hpp"

std::vector<SymbolID> BPETokenizer::encode(const std::string& text) const {
    std::vector<SymbolID> tokens;
    auto words = split_words(text);

    for (const auto& word : words) {
        std::vector<SymbolID> piece;
        for (uint8_t b : word) {
            piece.push_back(symbol_to_id.at(byte_to_string(b)));
        }

        bool changed;
        do {
            changed = false;
            for (size_t i = 0; i < piece.size() - 1; ++i) {
                auto it = merge_table.find({piece[i], piece[i+1]});
                if (it != merge_table.end()) {
                    piece[i] = it->second;
                    piece.erase(piece.begin() + i + 1);
                    changed = true;
                    break; 
                }
            }
        } while (changed);

        tokens.insert(tokens.end(), piece.begin(), piece.end());
    }

    return tokens;
}

std::vector<std::string> BPETokenizer::split_words(const std::string& text) const {
    return ::split_on_whitespace(text);
}