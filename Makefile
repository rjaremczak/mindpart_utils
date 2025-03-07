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
	$(CC) $(CCOPTS) -c -o $(OUT_DIR)/$@ $<

binup: $(SRC_DIR)/binup.c
	$(CC) $(CCOPTS) -o $(OUT_DIR)/$@ $<

crc16_test: $(TEST_SRC_DIR)/crc16_test.cpp $(SRC_DIR)/crc16.c
	$(CP) $(CCOPTS) -o $(OUT_DIR)/$@ -I$(SRC_DIR) -xc++ $(TEST_SRC_DIR)/crc16_test.cpp -xc $(SRC_DIR)/crc16.c

$(OUT_DIR)/%: $(TEST_SRC_DIR)/%.cpp
	$(CP) $(CCOPTS) -o $@ $<

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CCOPTS) -c -o $@ $<

clean:
	rm -rf $(OUT_DIR)/*
