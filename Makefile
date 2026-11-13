CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -fsanitize=address,undefined -MMD -MP \
            -Isrc -Isrc/cpu -Isrc/memory -Isrc/ppu -Isrc/apu
LDFLAGS  := -fsanitize=address,undefined
LDLIBS   := -lSDL2

SRCS := $(shell find src -name '*.cpp')
OBJS := $(SRCS:src/%.cpp=build/%.o)
DEPS := $(OBJS:.o=.d)

gb_emu: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build gb_emu

-include $(DEPS)
.PHONY: clean