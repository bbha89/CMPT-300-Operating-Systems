CC = gcc
CFLAGS = -g -Wall

# build target executable
TARGET = a2

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)