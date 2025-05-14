CC = g++
SOURCES = $(filter-out lab.cpp, $(wildcard *.cpp))
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = picRouting
# TEST = test/pic5x5.in
TEST = test/pic20x20.in
# TEST = test/pic60x60.in
TEST_OUT = $(TEST:.in=.out)

.PHONY: lab clean png run all check

all: $(EXECUTABLE)

check: $(TEST_OUT)
	test/pic_routing_verification $(TEST) $(TEST_OUT)

png: output.gp.png

output.gp.png: output.gp
	gnuplot output.gp

run output.gp $(TEST_OUT): $(EXECUTABLE) $(TEST) makefile
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