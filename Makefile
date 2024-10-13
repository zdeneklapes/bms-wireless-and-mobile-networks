# Define the compiler and compiler flags
CXX = g++
OPT_CXXFLAGS = -O0 -g # TODO: Change to -O3 for release
CXXFLAGS = -std=c++14 -Wall -Wextra -pedantic $(OPT_CXXFLAGS)

# Define the source files and header files
SRC_ENCODER = rds_encoder.cpp
SRC_DECODER = rds_decoder.cpp
HEADERS = rds_encoder.hpp rds_decoder.hpp

# Define the output binaries
BIN_ENCODER = rds_encoder
BIN_DECODER = rds_decoder

XLOGIN = xlapes02

# Default target
all: $(BIN_ENCODER) $(BIN_DECODER)

# Build encoder
$(BIN_ENCODER): $(SRC_ENCODER) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(BIN_ENCODER) $(SRC_ENCODER)

# Build decoder
$(BIN_DECODER): $(SRC_DECODER) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(BIN_DECODER) $(SRC_DECODER)

# Clean the build
clean:
	rm -f $(BIN_ENCODER) $(BIN_DECODER)

clean-all:
	rm -f $(BIN_ENCODER) $(BIN_DECODER)
	rm -f $(XLOGIN).pdf $(XLOGIN).zip

valgrind-encoder: $(BIN_ENCODER)
	valgrind --leak-check=yes ./$(BIN_ENCODER) -g 0A -pi 4660 -pty 5 -tp 1 -ms 0 -ta 1 -af 104.5,98.0 -ps "RadioXYZ"

valgrind-decoder: $(BIN_DECODER)
	valgrind --leak-check=yes ./$(BIN_DECODER)

run-encoder: $(BIN_ENCODER)
	#./$(BIN_ENCODER) -h
	./$(BIN_ENCODER) -g 0A -pi 4660 -pty 5 -tp 1 -ms 0 -ta 1 -af 104.5,98.0 -ps "RadioXYZ"
#	./$(BIN_ENCODER) --help

run-docker:
	docker compose run --remove-orphans --entrypoint /usr/bin/fish app

down-docker:
	docker compose down

pack:
	pandoc $(XLOGIN).md  -o $(XLOGIN).pdf
	zip -r $(XLOGIN).zip Makefile *.cpp *.hpp $(XLOGIN).pdf
	./check_zip.sh $(XLOGIN).zip

# Phony targets
.PHONY: all clean valgrind-encoder valgrind-decoder run-docker down-docker
