#include <fstream>
#include <random>
#include <string>
#include <iostream>

void writeTestFile(unsigned int size,
                   unsigned int propagationLoss,
                   unsigned int crossingLoss,
                   unsigned int bendingLoss,
                   unsigned int numNet) {
    std::string filename = "test/pic" + std::to_string(size) + "x" + std::to_string(size) + ".in";
    std::ofstream ofs(filename);
    if (!ofs) throw std::runtime_error("Cannot open file " + filename);

    ofs << "grid " << size << " " << size << "\n";
    ofs << "propagation loss " << propagationLoss << "\n";
    ofs << "crossing loss "   << crossingLoss   << "\n";
    ofs << "bending loss "    << bendingLoss    << "\n";
    ofs << "num net "         << numNet         << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, size - 1);

    for (unsigned int i = 0; i < numNet; ++i) {
        unsigned int x1, y1, x2, y2;
        do {
            x1 = dist(gen);
            y1 = dist(gen);
            x2 = dist(gen);
            y2 = dist(gen);
        } while (x1 == x2 && y1 == y2);
        ofs << i << " "
            << x1 << " " << y1 << " "
            << x2 << " " << y2 << "\n";
    }
}

int main(int argc, char* argv[]) {
    unsigned int size, propagationLoss, crossingLoss, bendingLoss, numNet;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " size [propagationLoss] [crossingLoss] [bendingLoss] [numNet]" << std::endl;
        return 1;
    }
    size = std::stoi(argv[1]);
    if (argc < 3) propagationLoss = 1;
    else propagationLoss = std::stoi(argv[2]);

    if (argc < 4) crossingLoss = 10;
    else crossingLoss = std::stoi(argv[3]);

    if (argc < 5) bendingLoss = 3;
    else bendingLoss = std::stoi(argv[4]);

    if (argc < 6) numNet = size;
    else numNet = std::stoi(argv[5]);

    writeTestFile(size, propagationLoss, crossingLoss, bendingLoss, numNet);

    return 0;
}