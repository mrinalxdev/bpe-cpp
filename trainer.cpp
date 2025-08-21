#include "bpe.hpp"
#include "utils.hpp"
#include <unordered_map>
#include <map>
#include <set>

void BPETokenizer::train(const std::string& raw_text, size_t target_vocab_size) {
    if (target_vocab_size <= 256) {
        throw std::invalid_argument("Vocabulary size must be > 256");
    }

    auto words = split_words(raw_text);
    std::vector<std::vector<SymbolID>> word_pieces;
    std::unordered_map<std::string, SymbolID> vocab = symbol_to_id;
    std::vector<std::string> reverse_vocab = id_to_symbol;

    if (vocab.empty()) {
        for (int i = 0; i < 256; ++i) {
            std::string byte_str = byte_to_string(static_cast<uint8_t>(i));
            vocab[byte_str] = static_cast<SymbolID>(i);
            if (reverse_vocab.size() <= static_cast<size_t>(i)) {
                reverse_vocab.resize(i + 1);
            }
            reverse_vocab[i] = byte_str;
        }
    }

    for (const auto& word : words) {
        std::vector<SymbolID> piece;
        for (uint8_t b : word) {
            piece.push_back(vocab[byte_to_string(b)]);
        }
        word_pieces.push_back(piece);
    }

    size_t vocab_size = vocab.size();
    merge_rules.clear();
    merge_table.clear();

    while (vocab_size < target_vocab_size) {
        std::map<std::pair<SymbolID, SymbolID>, size_t> pair_counts;
        for (const auto& piece : word_pieces) {
            for (size_t i = 0; i < piece.size() - 1; ++i) {
                pair_counts[{piece[i], piece[i+1]}]++;
            }
        }

        if (pair_counts.empty()) break;

        auto best = std::max_element(pair_counts.begin(), pair_counts.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });

        SymbolID a = best->first.first;
        SymbolID b = best->first.second;
        SymbolID new_id = static_cast<SymbolID>(vocab_size);
        std::string merged_str = reverse_vocab[a] + reverse_vocab[b];
        vocab[merged_str] = new_id;
        reverse_vocab.push_back(merged_str);
        merge_rules.push_back({a, b, new_id});
        merge_table[{a, b}] = new_id;
        for (auto& piece : word_pieces) {
            std::vector<SymbolID> new_piece;
            for (size_t i = 0; i < piece.size(); ++i) {
                if (i < piece.size() - 1 && piece[i] == a && piece[i+1] == b) {
                    new_piece.push_back(new_id);
                    ++i; // skip next
                } else {
                    new_piece.push_back(piece[i]);
                }
            }
            piece = std::move(new_piece);
        }

        ++vocab_size;
    }

    symbol_to_id = std::move(vocab);
    id_to_symbol = std::move(reverse_vocab);
}