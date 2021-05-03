CC = gcc
CFLAGS = -g -Wall

# build target executable
TARGET = myls

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)