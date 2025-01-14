TARGET_DIR = bin
OBJECT_DIR = obj

TARGET = $(TARGET_DIR)/main
SOURCES = main.cpp vector.cpp
OBJECTS = $(patsubst %.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))
CC = g++
CFLAGS = -Wall -O2

$(shell mkdir -p $(TARGET_DIR) $(OBJECT_DIR))

DEPS = $(SOURCES:.cpp=.d)

-include $(DEPS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(OBJECT_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) $(DEPS)

.PHONY: clean
