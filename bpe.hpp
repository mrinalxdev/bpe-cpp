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
