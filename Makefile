CXX = g++
CXXFLAGS = -std=c++17
TARGET = chip8
SRCS = main.cpp chip8.cpp
OBJS = $(SRCS:.cpp=.o)
BUILD = build/

all: $(BUILD) $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BUILD)$@ $(OBJS:%=$(BUILD)%)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $(BUILD)$@

clean:
	rm -rf $(BUILD)

.PHONY: all clean