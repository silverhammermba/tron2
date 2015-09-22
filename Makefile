BIN=tron2
LDLIBS=-lsfml-graphics -lsfml-window -lsfml-system

SRC=$(wildcard *.cpp)
OBJS=$(SRC:.cpp=.o)
DEPS=$(addprefix .,$(SRC:.cpp=.d))
CXXFLAGS:=-std=c++11 -Wall -Wextra -Wfatal-errors
CC=$(CXX)

$(BIN): $(OBJS)

.%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $^ > $@

-include $(DEPS)

.PHONY: clean

clean:
	rm -f $(BIN) $(OBJS)
