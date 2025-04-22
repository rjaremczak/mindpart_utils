SRC_DIR = src
TEST_SRC_DIR = test
OUT_DIR = build

CC = clang
CCOPTS = -o3 -Wall

SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/*_test.c)
TEST_EXECS = $(TEST_SRCS:$(TEST_SRC_DIR)/%_test.c=$(OUT_DIR)/%_test)

.phony: all rebuild clean binup baseconv parsefon

all: baseconv binup parsefon $(TEST_EXECS)

rebuild: clean default

baseconv: $(SRC_DIR)/baseconv.c
	$(CC) $(CCOPTS) -o $(OUT_DIR)/$@ $^

binup: $(SRC_DIR)/binup.c $(SRC_DIR)/crc16.c
	$(CC) $(CCOPTS) -o $(OUT_DIR)/$@ $^

parsefon: $(SRC_DIR)/parsefon.c $(SRC_DIR)/parsefon.c
	$(CC) $(CCOPTS) -o $(OUT_DIR)/$@ $^


$(OUT_DIR)/%_test: $(TEST_SRC_DIR)/%_test.c $(SRC_DIR)/%.c
	$(CC) $(CCOPTS) -o $@ -I$(SRC_DIR) $^

clean:
	rm -rf $(OUT_DIR)/*
