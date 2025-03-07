SRC_DIR = src
TEST_SRC_DIR = test
OUT_DIR = build

CC = clang
CP = clang++
CCOPTS = -o3 -Wall

SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/*.cpp)

rebuild: clean all

all: baseconv binup

baseconv: $(SRC_DIR)/baseconv.c
	$(CC) $(CCOPTS) -o $(OUT_DIR)/$@ $<

binup: $(SRC_DIR)/binup.c
	$(CC) $(CCOPTS) -o $(OUT_DIR)/$@ $<

test: $(TEST_SRCS:$(TEST_SRC_DIR)/%.cpp=$(OUT_DIR)/%)

$(OUT_DIR)/%: $(TEST_SRC_DIR)/%.cpp
	$(CP) $(CCOPTS) -o $@ $<

clean:
	rm -rf $(OUT_DIR)/*
