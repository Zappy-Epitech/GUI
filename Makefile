NAME        := bin/app
TEST_NAME   := bin/tests

CXX         := clang++
CC          := clang

CXXSTD      := -std=c++23
CSTD        := -std=gnu99
WARNINGS    := -Wall -Wextra -Wpedantic
DEBUG       := -g3
DEPFLAGS    := -MMD -MP

CXXFLAGS    := $(CXXSTD) $(WARNINGS) $(DEBUG) $(DEPFLAGS)
CFLAGS      := $(CSTD) $(WARNINGS) $(DEBUG) $(DEPFLAGS)
CPPFLAGS    := -I. $(shell pkg-config --cflags raylib) \
               -DFLECS_HI_COMPONENT_ID=128 \
               -DFLECS_ENTITY_PAGE_BITS=8 \
               -DFLECS_ID_DESC_MAX=16 \
               -DFLECS_EVENT_DESC_MAX=4
LDFLAGS     :=
LDLIBS      := $(shell pkg-config --libs raylib)

TEST_LDLIBS := -lcriterion

OBJ_DIR     := .build/obj
BIN_DIR     := bin

SRC_CPP     := $(shell find src -name '*.cpp' -not -path 'src/extern/*')
SRC_C       := $(shell find src -name '*.c' -not -path 'src/extern/*')
EXTERN_CPP  := $(shell find src/extern -name '*.cpp')
EXTERN_C    := $(shell find src/extern -name '*.c')
TEST_SRC    := $(shell find tests -name '*.cpp')

LIB_CPP     := $(filter-out src/main.cpp, $(SRC_CPP))
LIB_C       := $(SRC_C)

OBJ         := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_CPP)) \
               $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_C)) \
               $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(EXTERN_CPP)) \
               $(patsubst %.c,$(OBJ_DIR)/%.o,$(EXTERN_C))

LIB_OBJ     := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(LIB_CPP)) \
               $(patsubst %.c,$(OBJ_DIR)/%.o,$(LIB_C)) \
               $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(EXTERN_CPP)) \
               $(patsubst %.c,$(OBJ_DIR)/%.o,$(EXTERN_C))

TEST_OBJ    := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(TEST_SRC))

DEPS        := $(OBJ:.o=.d) $(TEST_OBJ:.o=.d)

.PHONY: all build run test clean fclean re

all: build

build: $(NAME)

run: $(NAME)
	./$(NAME)

test: $(TEST_NAME)
	./$(TEST_NAME)

$(NAME): $(OBJ) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(TEST_NAME): $(LIB_OBJ) $(TEST_OBJ) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS) $(TEST_LDLIBS)

$(OBJ_DIR)/src/extern/%.o: src/extern/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXSTD) $(DEBUG) $(DEPFLAGS) -w -c $< -o $@

$(OBJ_DIR)/src/extern/%.o: src/extern/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CSTD) $(DEBUG) $(DEPFLAGS) -w -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	@mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(BIN_DIR)

re: fclean build

-include $(DEPS)
