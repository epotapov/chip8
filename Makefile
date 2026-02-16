CXX = g++
CXXFLAGS = -std=c++17 -g
TARGET = chip8
SRCS = main.cpp chip8.cpp
OBJS = $(SRCS:.cpp=.o)
BUILD = build/

SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LIBS   := $(shell sdl2-config --libs)

all: $(BUILD) $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BUILD)$@ $(OBJS:%=$(BUILD)%) $(SDL2_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL2_CFLAGS) -c $< -o $(BUILD)$@

clean:
	rm -rf $(BUILD)

.PHONY: all clean