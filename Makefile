CC = clang++
CFLAGS = -Wall -Wextra -g -I src/backend

SRCS = src/backend/io.cpp src/backend/engine.cpp src/main.cpp 

OBJS = $(SRCS:.cpp=.o)

TARGET = server

all: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) $(TARGET)
