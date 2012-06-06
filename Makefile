all: main

main: main.o cycle.o helpers.o
	g++ -o launch main.o cycle.o helpers.o -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	g++ -c main.cpp -std=c++11

cycle.o: cycle.cpp cycle.hpp
	g++ -c cycle.cpp -std=c++11

helpers.o: helpers.cpp helpers.hpp
	g++ -c helpers.cpp -std=c++11

clean:
	rm -f *~ *.o *.out launch
