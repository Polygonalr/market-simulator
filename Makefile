CC = clang++
CFLAGS = -Wall -Wextra -g -I src/backend
DEBUG_FLAGS = -Wall -Wextra -g -O0 -I src/backend

SRCS = src/backend/io.cpp src/backend/engine.cpp src/main.cpp 

OBJS = $(SRCS:.cpp=.o)
DEBUG_OBJS = $(SRCS:.cpp=.debug.o)

TARGET = server
DEBUG = server_debug

all: $(TARGET)
debug: $(DEBUG)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.cpp
	$(CC) $(DEBUG_FLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(DEBUG): $(DEBUG_OBJS)
	$(CC) $(DEBUG_FLAGS) $^ -o $@

clean:
	rm -f $(OBJS) $(DEBUG_OBJS) $(TARGET) $(DEBUG)
