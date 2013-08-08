CXXFLAGS:=-std=c++11
TARGET:=tron2

tron2: main.o cycle.o helpers.o
	g++ -o $(TARGET) $^ -lsfml-graphics -lsfml-window -lsfml-system

cycle.o: cycle.hpp

helpers.o: helpers.hpp

clean:
	rm -f *~ *.o *.out $(TARGET)
