CMP = g++
FLAGS = -Wall -Wextra --std=c++17 -lOpenCL
OBJS = cl_manager.o
EXES = main
.PHONY: clean all

all: main

%.o: %.cpp
	$(CMP) -c $< -o $@ $(FLAGS)

main: main.cpp cl_manager.o
	$(CMP) $^ -o $@ $(FLAGS)

clean:
	rm -rf $(OBJS) $(EXES)
