CC = clang++
CFLAGS = -Wall -Wextra -g -std=c++2a

SRCS = util.cpp main.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = client

all: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) $(TARGET)
