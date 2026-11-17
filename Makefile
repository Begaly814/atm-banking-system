# Build the ATM banking system.
#
#   make          build ./atm
#   make run      build and run from the repo root (where the data file lives)
#   make clean    remove build artifacts
#
# The program reads sample_initial_condition.txt from the CURRENT directory,
# so run it from the repository root.

CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
SRC      := $(wildcard src/*.cpp)
OBJ      := $(SRC:.cpp=.o)
BIN      := atm

.PHONY: all run clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -Isrc -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
