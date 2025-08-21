#include "bpe.hpp"
#include "utils.hpp"

std::string BPETokenizer::decode(const std::vector<SymbolID>& tokens) const {
    std::string result;
    for (SymbolID id : tokens) {
        if (id >= id_to_symbol.size()) {
            throw std::runtime_error("Invalid token ID: " + std::to_string(id));
        }
        result += id_to_symbol[id];
    }

    if (!::is_valid_utf8(result)) {
        throw std::runtime_error("Decoded bytes are not valid UTF-8.");
    }

    return result;
}

// void BPETokenizer::save_vocab(const std::string& filename) const {
//     std::ostringstream oss;
//     for (size_t i = 0; i < id_to_symbol.size(); ++i) {
//         const std::string& symbol = id_to_symbol[i];
//         oss << i << "\t";
//         for (uint8_t b : symbol) {
//             oss << std::hex << std::uppercase << ((b < 16) ? "0" : "") << (int)b;
//         }
//         oss << "\n";
//     }
//     ::write_file(filename, oss.str());
// }


void BPETokenizer::save_vocab(const std::string& filename) const {
    std::ostringstream oss;
    for (size_t i = 0; i < id_to_symbol.size(); ++i) {
        const std::string& symbol = id_to_symbol[i];
        oss << i << "\t";
        for (uint8_t b : symbol) {
            oss << std::hex << std::uppercase << ((b < 16) ? "0" : "") << (int)b;
        }
        oss << "\n";
    }
    ::write_file(filename, oss.str());
}


// void BPETokenizer::load_vocab(const std::string& filename) {
//     std::string content = ::read_file(filename);
//     std::istringstream iss(content);
//     std::string line;
//     symbol_to_id.clear();
//     id_to_symbol.clear();

//     while (std::getline(iss, line)) {
//         auto tab = line.find('\t');
//         if (tab == std::string::npos) continue;
//         SymbolID id = static_cast<SymbolID>(std::stoul(line.substr(0, tab)));
//         std::string hex_str = line.substr(tab + 1);
//         std::string bytes;

//         for (size_t i = 0; i < hex_str.size(); i += 2) {
//             std::string byte_hex = hex_str.substr(i, 2);
//             uint8_t b = static_cast<uint8_t>(std::stoul(byte_hex, nullptr, 16));
//             bytes += static_cast<char>(b);
//         }

//         if (id >= id_to_symbol.size()) {
//             id_to_symbol.resize(id + 1);
//         }
//         id_to_symbol[id] = bytes;
//         symbol_to_id[bytes] = id;
//     }


//     merge_table.clear();
//     for (const auto& rule : merge_rules) {
//         merge_table[{rule.first, rule.second}] = rule.result;
//     }
// }

void BPETokenizer::load_vocab(const std::string& filename) {
    std::string content = ::read_file(filename);
    std::istringstream iss(content);
    std::string line;

    symbol_to_id.clear();
    id_to_symbol.clear();

    while (std::getline(iss, line)) {
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        auto tab = line.find('\t');
        if (tab == std::string::npos) {
            std::cerr << "Warning: Invalid line (no tab): " << line << std::endl;
            continue;
        }

        std::string id_str = line.substr(0, tab);
        std::string hex_str = line.substr(tab + 1);

        id_str.erase(id_str.find_last_not_of(" \t\r\n") + 1);
        hex_str.erase(0, hex_str.find_first_not_of(" \t\r\n"));

        if (id_str.empty() || hex_str.empty()) {
            continue;
        }

        SymbolID id;
        try {
            id = static_cast<SymbolID>(std::stoul(id_str));
        } catch (...) {
            std::cerr << "Warning: Invalid ID: " << id_str << std::endl;
            continue;
        }

        std::string bytes;
        try {
            for (size_t i = 0; i < hex_str.size(); i += 2) {
                if (i + 1 >= hex_str.size()) {
                    std::cerr << "Warning: Incomplete hex byte: " << hex_str.substr(i) << std::endl;
                    continue;
                }
                std::string byte_hex = hex_str.substr(i, 2);
                uint8_t b = static_cast<uint8_t>(std::stoul(byte_hex, nullptr, 16));
                bytes += static_cast<char>(b);
            }
        } catch (...) {
            std::cerr << "Warning: Failed to parse hex: " << hex_str << std::endl;
            continue;
        }

        if (id >= id_to_symbol.size()) {
            id_to_symbol.resize(id + 1);
        }
        id_to_symbol[id] = bytes;
        symbol_to_id[bytes] = id;
    }

    merge_table.clear();
    for (const auto& rule : merge_rules) {
        merge_table[{rule.first, rule.second}] = rule.result;
    }
}