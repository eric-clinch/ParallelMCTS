APP_NAME=mcts

SRCS= Board.cpp MCTS.cpp main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

default: $(APP_NAME)

CXX = g++ -m64 -std=c++11
CXXFLAGS = -I. -O3 -Wall -Wno-unknown-pragmas

# Compilation Rules
$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAME)

# For a given rule:
# $< = first prerequisite
# $@ = target
# $^ = all prerequisite

