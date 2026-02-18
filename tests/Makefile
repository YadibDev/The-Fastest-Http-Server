CPP = g++
CPPFLAGS = -Wall -Wextra -Werror -std=c++11

SRC = test_header.cpp
TARGET = header_test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CPP) $(CPPFLAGS) -o $(TARGET) $^

clean:
	rm -f $(TARGET)

.PHONY: all clean
