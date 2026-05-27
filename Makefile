NAME        := bin/app
TEST_NAME   := bin/tests

CC          := clang
CXX         := clang++

CSTD        := -std=gnu99
CXXSTD      := -std=c++23
WARNINGS    := -Wall -Wextra -Wpedantic
DEBUG       := -g3
DEPFLAGS    := -MMD -MP

CFLAGS      := $(CSTD) $(WARNINGS) $(OPTIM) $(DEBUG) $(DEPFLAGS)
CXXFLAGS    := $(CXXSTD) $(WARNINGS) $(OPTIM) $(DEBUG) $(DEPFLAGS)
CPPFLAGS    := -I. $(shell pkg-config --cflags raylib)
LDFLAGS     :=
LDLIBS      := $(shell pkg-config --libs raylib)

TEST_LDLIBS := -lcriterion

OBJ_DIR     := .build/obj
BIN_DIR     := bin

ALL_CPP_FILES := $(shell find . -type f -name '*.cpp' -not -path "./.build/*")
ALL_CPP_FILES := $(ALL_CPP_FILES:./%=%)
ALL_C_FILES := $(shell find . -type f -name '*.c' -not -path "./.build/*")
ALL_C_FILES := $(ALL_C_FILES:./%=%)

SRC_CPP   := $(filter-out tests/% src/extern/flecs.cpp, $(ALL_CPP_FILES))
SRC_C     := $(filter-out tests/%, $(ALL_C_FILES))
TEST_SRC  := $(filter tests/%, $(ALL_CPP_FILES))
LIB_CPP   := $(filter-out src/main.cpp, $(SRC_CPP))
LIB_C     := $(SRC_C)

OBJ       := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_CPP)) $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_C))
LIB_OBJ   := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(LIB_CPP)) $(patsubst %.c,$(OBJ_DIR)/%.o,$(LIB_C))
TEST_OBJ  := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_SRC))

FORMAT_FILES := $(shell find . -type f \( -name '*.cpp' -o -name '*.h' \) -not -path "./.build/*" -not -path "./docs/*")

DEPS := $(OBJ:.o=.d) $(LIB_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

.PHONY: all build run test clean fclean re

all: build

build: $(NAME)

.PHONY: run
run: $(NAME)
	./$(NAME)

.PHONY: test
test: $(TEST_NAME)
	./$(TEST_NAME)

$(NAME): $(OBJ) | $(BIN_DIR)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

$(TEST_NAME): $(LIB_OBJ) $(TEST_OBJ) | $(BIN_DIR)
	$(CXX) $(LIB_OBJ) $(TEST_OBJ) -o $@ $(LDFLAGS) $(LDLIBS) $(TEST_LDLIBS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	rm -rf $(BIN_DIR)

.PHONY: re
re: fclean build

-include $(DEPS)
