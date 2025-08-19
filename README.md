# Byte Pair Encoding (BPE) Tokenizer: Maths and Logic

## Introduction to BPE

Byte Pair Encoding (BPE) is a data compression technique adapted for subword tokenization in natural language processing (NLP). It was introduced by Philip Gage in 1994 for compression and later popularized in NLP by Sennrich et al. in 2016 for neural machine translation. The core idea is to iteratively merge the most frequent pairs of symbols (bytes or characters) to build a vocabulary that balances between character-level and word-level representations.

BPE starts with a base vocabulary of individual bytes (or characters) and learns a set of merge rules based on frequency counts. These rules are applied to tokenize new text into subwords. This approach handles rare words and out-of-vocabulary issues effectively by breaking them into known subparts.

Mathematically, BPE can be viewed as a greedy algorithm that minimizes the number of tokens needed to represent a corpus while maximizing the coverage of frequent patterns. It relates to information theory concepts like entropy, where frequent merges reduce the average code length (similar to Huffman coding but pair-based).

## Core Logic and Algorithm

### 1. Initialization
- Begin with the raw text corpus as a sequence of bytes (UTF-8 encoded, typically).
- The initial vocabulary $ V $ consists of all unique bytes (256 for ASCII/UTF-8).
- Represent each word in the corpus as a sequence of its bytes, often with a special end-of-word symbol (e.g., '</w>') to distinguish subwords within words.

For a corpus $ C = \{w_1, w_2, \dots, w_n\} $ where each $ w_i $ is a word, preprocess to:
$$
w_i' = \text{list of bytes in } w_i + \text{ '</w>'}
$$
Count the frequency of each word $ f(w_i) $, so the total occurrences of byte sequences are weighted by word frequencies.

### 2. Pair Frequency Calculation
- For the current representation of the corpus (as lists of symbols from $ V $), compute the frequency of every adjacent pair of symbols.
- Let \( S \) be the set of all symbol sequences in the preprocessed corpus.
- For each pair \( (a, b) \) where \( a \) and \( b \) are consecutive symbols in some sequence, the pair frequency is:
\[
freq(a, b) = \sum_{s \in S} f(s) \cdot \text{count of } (a, b) \text{ in } s
\]
Here, \( f(s) \) is the frequency of the word corresponding to sequence \( s \).

This is essentially a bigram count over the symbol level, weighted by word frequencies.

### 3. Merging Step
- Identify the pair \( (a^*, b^*) \) with the maximum frequency:
\[
(a^*, b^*) = \arg\max_{(a,b)} freq(a, b)
\]
- Merge this pair into a new symbol \( ab \).
- Update the vocabulary: \( V \leftarrow V \cup \{ab\} \).
- Replace all occurrences of the pair \( (a, b) \) in the corpus sequences with the new symbol \( ab \). Note: Only adjacent pairs are merged; non-adjacent remain separate.
- This replacement affects future pair frequencies, as new pairs may form (e.g., if 'a b' becomes 'ab', then adjacent symbols change).

### 4. Iteration
- Repeat steps 2-3 for a fixed number of merges \( m \) (a hyperparameter, e.g., 32k-64k for large models).
- The process is greedy: always merge the current most frequent pair, without backtracking.
- After \( m \) merges, the final vocabulary is \( V \) (initial bytes + merged symbols), and the merge rules are the ordered list of merges.

The number of merges controls the vocabulary size: \( |V| = 256 + m \) (approximately, ignoring duplicates).

### 5. Tokenization (Encoding)
- For a new word \( w \), represent it as initial byte sequence + '</w>'.
- Apply the merge rules in the order they were learned: For each rule \( (a, b) \to ab \), scan the sequence and replace all adjacent \( a b \) with \( ab \).
- Continue until no more merges apply.
- The final sequence of symbols is the tokenization.
- Output token IDs based on a mapping from \( V \) to integers.

This is deterministic and order-dependent: earlier merges are applied first, which can affect later ones.

### 6. Decoding
- Given a sequence of tokens (subwords), concatenate them directly.
- Remove the '</w>' symbols and replace with spaces (or handle as word boundaries).
- Since merges are reversible (each merged symbol can be split back), decoding is straightforward.

## Mathematical Formulation

BPE can be formalized as optimizing a compression objective implicitly. Let’s denote:

- Corpus as a string \( S \) over alphabet \( \Sigma \) (bytes).
- A merge operation \( \mu: (a,b) \to c \) where \( c \) is new symbol.

The goal is to find a sequence of merges \( \mu_1, \mu_2, \dots, \mu_m \) that minimizes the length of the encoded corpus:
\[
L(S, \{\mu_i\}) = \sum_{tokens \in \text{encode}(S)} 1
\]
But since direct optimization is NP-hard, BPE uses greedy approximation: at each step, choose \( \mu \) that maximally reduces \( L \) (i.e., merges the most frequent pair, reducing length by \( freq(a,b) - 1 \) positions per merge, but actually by \( freq(a,b) \) since each pair becomes one symbol).

More precisely, each merge reduces the total number of symbols in the corpus by \( freq(a,b) \), because each pair instance is replaced by one symbol.

Thus, the greedy choice maximizes the immediate reduction:
\[
\Delta L = -freq(a,b)
\]

In information theory terms, BPE approximates a variable-length code where frequent n-grams get shorter representations. The average token length relates to entropy:
\[
H = -\sum p(x) \log p(x)
\]
But BPE doesn't explicitly use probabilities; it's frequency-based.

For probabilities: If we model the corpus as a sequence generated by a source, BPE builds a dictionary that reduces the expected code length.

## Example: Step-by-Step

Consider a tiny corpus: "low low lowest newer new".

Preprocess (add '</w>', assume space-separated words, bytes as chars for simplicity):

Sequences:
- l o w </w> (freq 2, since "low" appears twice)
- l o w e s t </w> (freq 1)
- n e w e r </w> (freq 1)
- n e w </w> (freq 1)

Initial vocabulary: {l, o, w, e, s, t, n, r, </w>}

Pair frequencies:
- (l,o): 3 (from low x2, lowest)
- (o,w): 3
- (w,</w>): 2 (low x2)
- (w,e): 1 (lowest)
- (e,s): 1
- (s,t): 1
- (t,</w>): 1
- (n,e): 2 (newer, new)
- (e,w): 1 (newer), wait no: newer is n e w e r </w>, so (n,e), (e,w), (w,e), (e,r), (r,</w>)
- (w,e): 1 (from newer) +1 (lowest) =2
- etc.

Compute properly:

All pairs:
From "low</w>" x2: (l,o)x2, (o,w)x2, (w,</w>)x2
From "lowest</w>": (l,o), (o,w), (w,e), (e,s), (s,t), (t,</w>)
From "newer</w>": (n,e), (e,w), (w,e), (e,r), (r,</w>)
From "new</w>": (n,e), (e,w), (w,</w>)

Total:
- (l,o): 3
- (o,w): 3
- (w,</w>): 3 (low x2 + new)
- (w,e): 2 (lowest + newer)
- (e,s):1
- (s,t):1
- (t,</w>):1
- (n,e):2
- (e,w):2 (newer + new)
- (e,r):1
- (r,</w>):1

Most frequent: (l,o)=3, (o,w)=3, (w,</w>)=3. Assume ties broken arbitrarily, say merge (l,o) -> 'lo'

Update sequences:
- lo w </w> x2
- lo w e s t </w>
- n e w e r </w>
- n e w </w>

New pairs:
- (lo,w):3 (from low x3 effectively)
- etc.

Next merge: say (lo,w) -> 'low', and so on.

Continuing merges would build 'low</w>', 'new', etc.

For tokenization of "lowest": start as l o w e s t </w>, apply merges if learned: if 'lo' merged, becomes lo w e s t </w>, then if 'low' merged, low e s t </w>, etc.

## Advantages and Limitations

- **Logic Behind Efficiency**: Greedy merging captures frequent subwords, reducing vocabulary size while handling morphology (e.g., 'un' + 'do' + 'able').
- **Maths of Coverage**: With \( m \) merges, it can represent up to \( 256^m \) possible strings, but in practice, it's limited by corpus diversity.
- **No Probabilistic Model**: Unlike learned embeddings, BPE is rule-based; no direct likelihood maximization.
- Limitations: Order of merges matters; ties can lead to different vocabularies. Doesn't handle semantics, just frequencies.

## Further Maths: Relation to Compression

BPE is similar to dictionary compression. The compression ratio is:
\[
CR = \frac{\text{original bytes}}{\text{encoded tokens} \times \log_2 |V|}
\]
But in NLP, we care about token count for model input length.

In terms of Kolmogorov complexity, BPE approximates the minimal description length by building a grammar of merges.

For deeper dives, see original papers, but this covers the core logic and maths.