CC = gcc
CFLAGS = -g -Wall

# build target executable
TARGET = lets-talk

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c list.c -lpthread

valgrind:
	valgrind --leak-check=full ./lets-talk 6000 127.0.0.1 6001

clean:
	$(RM) $(TARGET)
