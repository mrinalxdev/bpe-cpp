CXX=clang++ 
CXXFLAGS=-std=c++17 -O2 -Wall -Wextra -pedantic
SOURCES=tokenizer.cpp trainer.cpp encoder.cpp decoder.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=bpe_tokenizer

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) bpe_vocab.txt

run: $(TARGET)
	./$(TARGET)