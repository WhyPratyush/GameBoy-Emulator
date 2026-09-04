CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Wformat=2 -Wold-style-cast -g -fsanitize=address,undefined
LDFLAGS  := -fsanitize=address,undefined
LDLIBS   := -lSDL2

SRCS     := $(wildcard *.cpp)
OBJS     := $(SRCS:.cpp=.o)
TARGET   := gb_emu

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)