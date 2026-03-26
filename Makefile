
BUILD_DIR:=build
SRC_DIR:=src

SRC_FILES:=$(wildcard $(SRC_DIR)/*.cpp)
H_FILES:=$(wildcard $(SRC_DIR)/*.h)
OBJ_FILES:=$(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o)

EXE:=$(BUILD_DIR)/bin/trace

OBJS:=$(EXE)

CXXFLAGS:=-x c++ -fPIC -Wextra -Wall -ggdb -fsanitize=address -fsanitize=undefined
LINK_FLAGS:=-lasan -lubsan
CC:=g++

.PHONY: build
build: $(OBJS)

.PHONY: build
clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) -c $(CXXFLAGS) -o $@ $^ -I .

$(EXE): $(OBJ_FILES) $(H_FILES)
	mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LINK_FLAGS)
