#include "parser.h"
#include "routing.h"
#include <iostream>
#include <fstream>

void printCircuit(const Circuit& circuit) {
    std::cout << "grid " << circuit.gridX << " " << circuit.gridY << std::endl;
    std::cout << "propagation loss " << circuit.propagationLoss << std::endl;
    std::cout << "crossing loss " << circuit.crossingLoss << std::endl;
    std::cout << "bending loss " << circuit.bendingLoss << std::endl;
    std::cout << "num net " << circuit.nets.size() << std::endl;
    for (const auto& net : circuit.nets) {
        std::cout << net.id << " " << net.x1 << " " << net.y1 << " " << net.x2 << " " << net.y2 << std::endl;
    }
}

void writeGP(const std::vector<Net>& netList, const std::string& filename, unsigned int girdX, unsigned int gridY) {
    // Predefined color palette (will cycle if more nets than colors)
    const std::vector<std::string> colors = {
        "red", "green", "blue", "orange", "purple",
        "cyan", "magenta", "yellow", "black", "gray"
    };

    std::ofstream gp(filename);
    if (!gp) {
        throw std::runtime_error("Cannot open file " + filename + ".gp for writing");
    }

    gp << "set terminal pngcairo size " 
        << girdX * 32 + 144 << "," 
        << gridY * 32 + 106 << "\n";
    gp << "set output '" << filename << ".png'\n";
    gp << "set title 'Net Plot'\n";
    gp << "set xlabel 'X'\n";
    gp << "set ylabel 'Y'\n";
    gp << "set key off\n\n";
    gp << "set xtics 1\n";
    gp << "set ytics 1\n";
    gp << "set grid xtics ytics\n";
    gp << "set xrange [-1:" << girdX + 1 << "]\n";
    gp << "set yrange [-1:" << gridY + 1 << "]\n";

    // Begin plot command
    gp << "plot ";
    size_t idx = 0;
    for (auto it = netList.begin(); it != netList.end(); ++it, ++idx) {
        const std::string& color = colors[idx % colors.size()];
        gp << "'-' with lines lt rgb '" << color << "' lw 2";
        if (std::next(it) != netList.end()) {
            gp << ", \\\n     ";
        } else {
            gp << "\n\n";
        }
    }

    // Inline data for each net
    idx = 0;
    for (const auto& net : netList) {
        for (const auto& p : net.points) {
            gp << p.x << " " << p.y << "\n";
        }
        gp << "e\n";
        ++idx;
    }

    gp.close();
}

void writeOutput(const std::vector<Net>& netList, const std::string& filename) {
    std::ofstream out{filename};

    for (const auto& net : netList) {
        out << net.id << " " << net.points.size() - 1 << "\n";
        auto it = net.points.begin();
        if (it == net.points.end()) continue;
        auto x = it->x, y = it->y;
        while ((++it) != net.points.end()) {
            out << x << " " << y << " " << it->x << " " << it->y << "\n";
            x = it->x; y = it->y;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [input file name] [output file name]" << std::endl;
        return 1;
    }

    auto circuit = parse(argv[1]);

    auto netList = route(circuit);

    writeOutput(netList, argv[2]);

    writeGP(netList, "output.gp", circuit.gridX, circuit.gridY);

    return 0;
}