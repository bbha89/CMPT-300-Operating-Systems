CC = gcc
CFLAGS = -g -Wall

# build target executable
TARGET = my_current_processes

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)