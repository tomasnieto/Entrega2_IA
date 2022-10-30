CC = g++

CFLAGS  = -g -Wall

TARGET = gbj

all: $(TARGET)

$(COMPILER): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET).cpp

run:
	./$(TARGET) $(SOLVE)
clean:
	$(RM) $(TARGET)