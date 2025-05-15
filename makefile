CC = g++
SOURCES = $(filter-out lab.cpp testGenerator.cpp, $(wildcard *.cpp))
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = picRouting
# TEST = test/pic5x5
TEST = test/pic20x20
# TEST = test/pic60x60
# TEST = test/pic512x512
TEST_IN = $(TEST).in
TEST_OUT = $(TEST).out
TEST_GP = $(TEST).gp
TEST_PNG = $(TEST).png

.PHONY: lab clean png run all check generate_test

all: $(EXECUTABLE) testGenerator

check_all: test/pic5x5.out test/pic20x20.out test/pic60x60.out
	test/pic_routing_verification test/pic5x5.in test/pic5x5.out
	test/pic_routing_verification test/pic20x20.in test/pic20x20.out
	test/pic_routing_verification test/pic60x60.in test/pic60x60.out

generate_test: testGenerator
	./testGenerator

testGenerator: testGenerator.cpp
	g++ -std=c++11 -O3 testGenerator.cpp -o testGenerator;

check: $(TEST_OUT)
	test/pic_routing_verification $(TEST_IN) $(TEST_OUT)

png: $(TEST_PNG)

$(TEST_PNG): $(TEST_GP)
	gnuplot $(TEST_GP)

run $(TEST_GP) $(TEST_OUT): $(EXECUTABLE) $(TEST_IN) makefile
	./$(EXECUTABLE) $(TEST_IN) $(TEST_OUT)

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