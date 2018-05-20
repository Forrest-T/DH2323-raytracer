CMP = g++
FLAGS = -Wall -Wextra --std=c++17 -lOpenCL
OBJS = io.o
EXES = main
.PHONY: clean all

all: main

%.o: %.cpp
	$(CMP) -c $< -o $@ $(FLAGS)

main: main.cpp io.o getInfo.o
	$(CMP) $^ -o $@ $(FLAGS)

clean:
	rm -rf $(OBJS) $(EXES)
