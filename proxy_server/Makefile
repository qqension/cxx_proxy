CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include -I./third_party
LDFLAGS = -pthread

SRCS = src/main.cpp src/proxy_server.cpp src/filter_manager.cpp src/web_ui.cpp src/logger.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = proxy_server

all: $(TARGET) logs_dir

logs_dir:
	mkdir -p logs

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean logs_dir 