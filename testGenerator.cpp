#include <fstream>
#include <random>
#include <string>

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

int main() {
    writeTestFile(512, 1, 10, 3, 512);

    return 0;
}