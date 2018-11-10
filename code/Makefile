APP_NAME=mcts

SRCS= Board.cpp Move.h Strategy.h UCB1.h UtilityNode.h main.h
OBJS=$(subst .cpp,.o,$(SRCS))
OBJS:=$(subst .h,.o,$(OBJS))

default: $(APP_NAME)

CXX = g++ -m64 -std=c++11
CXXFLAGS = -I. -O3 -Wall -Wno-unknown-pragmas

# Compilation Rules
$(APP_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

%.o: %.h
	$(CXX) $< $(CXXFLAGS) -c -o $@

clean:
	/bin/rm -rf *~ *.o $(APP_NAME)

# For a given rule:
# $< = first prerequisite
# $@ = target
# $^ = all prerequisite

