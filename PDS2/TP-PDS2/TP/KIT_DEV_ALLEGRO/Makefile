PASTA_KIT_ALLEGRO = ../KIT_DEV_ALLEGRO
CC = g++
CFLAGS = -std=c++11 -Wall
TARGET = $(PASTA_KIT_ALLEGRO)/vpl_execution.exe

BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
PATH_ALLEGRO = $(PASTA_KIT_ALLEGRO)/allegro-5.0.10-mingw-4.7.0
LIBS = -lallegro-5.0.10-monolith-mt

SRCS := $(wildcard $(SRC_DIR)/entidades/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/entidades/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

$(TARGET): $(OBJS) $(BUILD_DIR)/main.o
	$(CC) $(CFLAGS) $^ -o $@ -L$(PATH_ALLEGRO)/lib $(LIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/entidades/%.cpp $(INCLUDE_DIR)/%.hpp
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(PATH_ALLEGRO)/include -c $< -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -I$(PATH_ALLEGRO)/include -c $< -o $@

.PHONY: make
make: $(TARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET)
