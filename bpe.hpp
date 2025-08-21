#pragma once

#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using SymbolID = uint32_t;

struct MergeRule {
  SymbolID first;
  SymbolID second;
  SymbolID result;

  bool operator==(const MergeRule &other) const {
    return first == other.first && second == other.second &&
           result == other.result;
  }
};

struct MergeRuleHash {
  size_t operator()(const MergeRule &m) const {
    return std::hash<SymbolID>()(m.first) ^
           (std::hash<SymbolID>()(m.second) << 1) ^
           (std::hash<SymbolID>()(m.result) << 2);
  }
};

struct PairHash {
  size_t operator()(const std::pair<SymbolID, SymbolID>& p) const {
    return std::hash<SymbolID>()(p.first) ^ (std::hash<SymbolID>()(p.second) << 1);
  }
};


// {comment} tokenizer class where the main function of training is to be
// written mostly handled by the vector, the training and the encoding part
// still figuring out the decoding part but for sure will get there too
//
// So here is the encoding logic -- HEHE ^^
// encoding means --> means converting a string of text into a list of integer
// token IDs, using the rules leaned during training Fast (O(n) per word in
// practise)
class BPETokenizer {
public:
  BPETokenizer();
  explicit BPETokenizer(const std::string &vocab_file);

  void train(const std::string &text, size_t vocab_size);

  std::vector<SymbolID> encode(const std::string &text) const;
  std::string decode(const std::vector<SymbolID> &tokens) const;


  void save_vocab(const std::string& filename) const;
  void load_vocab(const std::string& filename);


private:
  std::unordered_map<std::string, SymbolID> symbol_to_id;
  std::vector<std::string> id_to_symbol;

  std::vector<MergeRule> merge_rules;

  std::unordered_map<std::pair<SymbolID, SymbolID>, SymbolID, PairHash> merge_table;

  std::vector<std::string> split_words(const std::string& text) const;

  friend class BPETrainer;
  friend class BPEEncoder;
  friend class BPEDecoder;
};