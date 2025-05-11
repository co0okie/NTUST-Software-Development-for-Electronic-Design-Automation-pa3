#include "parser.h"
#include <iostream>
#include <fstream>

void printCircuit(const Circuit& circuit) {
    std::cout << "grid " << circuit.girdX << " " << circuit.girdY << std::endl;
    std::cout << "propagation loss " << circuit.propagationLoss << std::endl;
    std::cout << "crossing loss " << circuit.crossingLoss << std::endl;
    std::cout << "bending loss " << circuit.bendingLoss << std::endl;
    std::cout << "num net " << circuit.nets.size() << std::endl;
    for (const auto& net : circuit.nets) {
        std::cout << net.x1 << " " << net.y1 << " " << net.x2 << " " << net.y2 << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [input file name] [output file name]" << std::endl;
        return 1;
    }

    std::ofstream output{argv[2]};

    auto circuit = parse(argv[1]);

    printCircuit(circuit);

    return 0;
}