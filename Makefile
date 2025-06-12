CC = gcc
CC_FLAGS = -Wall -g -ggdb -lpthread
BUILD_DIR = build
SRC_DIR = src

SRC = $(wildcard $(SRC_DIR)/*.c)

OBJ = $(patsubst $(SRC_DIR)/*.c, $(BUILD_DIR)/%.o, $(SRC))

TARGET = mcproxy

all: $(TARGET)

$(TARGET): $(OBJ) $(BUILD_DIR)
	$(CC) $(OBJ) $(CC_FLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c %(BUILD_DIR)
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	rm -rf *.o build/*.o proxmc
