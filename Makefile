CC := clang
CFLAGS := -static -O2 -std=c17

TARGET_DIR := /home/anansi/myspace/google/vm/rootfs/bin
TARGET := $(TARGET_DIR)/aapk

SRCS := $(wildcard *.c) 

# 默认构建规则
build: 
	@echo "building..."
	@$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# 清理规则
clean:
	@echo "remove"
	@rm -rf $(TARGET)

.PHONY: build clean