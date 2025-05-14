#include "parser.h"
#include <sstream>
#include <iostream>
#include <fstream>

Circuit parse(const std::string& fileName) {
    std::ifstream input{fileName};
    if (!input) std::cerr << "Error opening files" << std::endl;

    struct Parser {
        Parser(std::ifstream& input) : s{input} {}

        std::ifstream& s;

        unsigned int parseUInt() {
            unsigned int value;
            if (!(s >> value)) {
                std::cerr << "Error: expected unsigned int but got '" 
                          << parseWord() << "'\n";
                std::exit(1);
            }
            return value;
        }

        std::string parseWord() {
            std::string word;
            s >> word;
            return word;
        }

        std::vector<Circuit::Net> parseNet() {
            std::vector<Circuit::Net> nets{parseUInt()};

            for (int i = 0; i < nets.size(); ++i) {
                nets[i].id = parseUInt();
                nets[i].x1 = parseUInt();
                nets[i].y1 = parseUInt();
                nets[i].x2 = parseUInt();
                nets[i].y2 = parseUInt();
            }

            return nets;
        }

        Circuit parse() {
            Circuit circuit;

            std::string token;
            while (s >> token) {
                if (token == "grid") {
                    circuit.gridX = parseUInt();
                    circuit.gridY = parseUInt();
                }
                else if (token == "propagation" && parseWord() == "loss") {
                    circuit.propagationLoss = parseUInt();
                }
                else if (token == "crossing" && parseWord() == "loss") {
                    circuit.crossingLoss = parseUInt();
                }
                else if (token == "bending" && parseWord() == "loss") {
                    circuit.bendingLoss = parseUInt();
                } 
                else if (token == "num" && parseWord() == "net") {
                    circuit.nets = parseNet();
                }
                else {
                    std::cerr << "unexpected " << parseWord() << "\n";
                    exit(1);
                }
            }

            return circuit;
        }
    };

    return Parser{input}.parse();
}