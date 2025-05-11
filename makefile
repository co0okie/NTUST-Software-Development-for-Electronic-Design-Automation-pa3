CC = g++
SOURCES = main.cpp parser.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = picRouting
TEST = test/pic60x60.in

.PHONY: lab clean

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE) $(TEST) $(TEST:.in=.out)

$(EXECUTABLE): $(OBJECTS)
	g++ -o $(EXECUTABLE) $(OBJECTS)

%.o: %.cpp
	g++ -std=c++11 -O3 -c $< -o $@

lab: lab.out
	./lab.out

lab.out: lab.cpp
	g++ lab.cpp -o lab.out;

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)