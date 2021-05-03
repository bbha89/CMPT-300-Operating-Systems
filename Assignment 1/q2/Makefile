CC = gcc
CFLAGS = -g -Wall

# build target executable
TARGET = my_memory_util

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)