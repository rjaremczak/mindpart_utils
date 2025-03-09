SRC_DIR = src
TEST_SRC_DIR = test
OUT_DIR = build

CC = clang
CP = clang++
CCOPTS = -o3 -Wall

SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_SRC_DIR)/*_test.cpp)
TEST_EXECS = $(TEST_SRCS:$(TEST_SRC_DIR)/%_test.cpp=$(OUT_DIR)/%_test)

default: baseconv binup

rebuild: clean default

baseconv: $(SRC_DIR)/baseconv.c
	$(CC) $(CCOPTS) -c -o $(OUT_DIR)/$@ $<

binup: $(SRC_DIR)/binup.c $(SRC_DIR)/crc16.c
	$(CC) $(CCOPTS) -o $(OUT_DIR)/$@ $^

tests: $(TEST_EXECS)

$(OUT_DIR)/%_test: $(TEST_SRC_DIR)/%_test.cpp $(SRC_DIR)/%.c
	$(CP) $(CCOPTS) -o $@ -I$(SRC_DIR) -xc++ $(word 1,$^) -xc $(word 2,$^)

clean:
	rm -rf $(OUT_DIR)/*
