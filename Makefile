SRC_DIR = src
OUT_DIR = build

CC = clang
CCOPTS = -o3 -Wall

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

rebuild: clean all

all: $(SRC_FILES:$(SRC_DIR)/%.c=$(OUT_DIR)/%)

clean:
	rm -rf $(OUT_DIR)/*

$(OUT_DIR)/%: $(SRC_DIR)/%.c
	$(CC) $(CCOPTS) -o $@ $<

