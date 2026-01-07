CC = clang++
CFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic -Wno-unused-result -Wconversion -Wvla

TARGET = sokoban

all: $(TARGET)

$(TARGET): main.cpp
	$(CC) $(CFLAGS) main.cpp -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
