#include "bpe.hpp"
#include "utils.hpp"
#include <iostream>

int main() {
    try {
        std::string training_text = 
            "Hello world! This is a test.\n"
            "Hello BPE tokenizer from scratch.\n"
            "Tokenization is fun. BPE merges common pairs.\n"
            "Testing merge: aa bb aa bb cc aa bb.";

        BPETokenizer tokenizer;
        std::cout << "Training BPE tokenizer...\n";
        tokenizer.train(training_text, 300);  

        std::string test_text = "But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was born and I will give you a complete account of the system, and expound the actual teachings of the great explorer of the truth, the master-builder of human happiness. No one rejects, dislikes, or avoids pleasure itself, because it is pleasure, but because those who do not know how to pursue pleasure rationally encounter consequences that are extremely painful. Nor again is there anyone who loves or pursues or desires to obtain pain of itself, because it is pain, but because occasionally circumstances occur in which toil and pain can procure him some great pleasure. To take a trivial example, which of us ever undertakes laborious physical exercise, except to obtain some advantage from it? But who has any right to find fault with a man who chooses to enjoy a pleasure that has no annoying consequences, or one who avoids a pain that produces no resultant pleasure?";
        auto tokens = tokenizer.encode(test_text);

        std::cout << "Original: " << test_text << "\n";
        std::cout << "Tokens: ";
        for (auto t : tokens) {
            std::cout << t << " ";
        }
        std::cout << "\n";

        std::string decoded = tokenizer.decode(tokens);
        std::cout << "Decoded: " << decoded << "\n";
        tokenizer.save_vocab("bpe_vocab.txt");
        std::cout << "Vocabulary saved to bpe_vocab.txt\n";
        // BPETokenizer loaded_tokenizer("bpe_vocab.txt");
        // auto tokens2 = loaded_tokenizer.encode("Hello test");
        // std::cout << "Re-encoded with loaded tokenizer: ";
        // for (auto t : tokens2) std::cout << t << " ";
        // std::cout << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}