all: tron2

tron2: tron2.o
	g++ -o launch tron2.o -lsfml-graphics -lsfml-window -lsfml-system

tron2.o: tron2.cpp
	g++ -c tron2.cpp -std=c++11

clean:
	rm -f *~ *.o *.out launch
