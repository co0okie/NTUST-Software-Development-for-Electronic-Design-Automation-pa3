#include "routing.h"
#include <set>
#include <list>
#include <iostream>
#include <algorithm>

// #define DEBUG 1
int DEBUG = 1;

struct Node {
    unsigned int x, y;
    unsigned int cost;
    unsigned int estimate;
    bool lock = false;
    const Node* prev;
    Node(
        const unsigned int x, const unsigned int y, 
        const unsigned int cost, const unsigned int estimate, 
        const bool lock = false, const Node* prev = nullptr
    ) : x{x}, y{y}, cost{cost}, estimate{estimate}, lock{lock}, prev{prev} {}
};

struct NetLoss {
    Net net;
    unsigned int loss;
};

int upDownLeftRight[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

void printGrid(const std::vector<std::vector<unsigned int>>& grid) {
    for (size_t i = 0; i < grid[0].size(); i++) {
        for (size_t j = 0; j < grid.size(); j++) {
            std::cout << grid[j][i] << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

unsigned int estimate(
    const Node* prev,
    const unsigned int x1, const unsigned int y1, 
    const unsigned int x2, const unsigned int y2, 
    const unsigned int propagationLoss, const unsigned int bendingLoss
) {
    const unsigned int dx = x1 > x2 ? x1 - x2 : x2 - x1;
    const unsigned int dy = y1 > y2 ? y1 - y2 : y2 - y1;
    unsigned int loss = propagationLoss * (dx + dy);
    if (x1 != x2 && y1 != y2) loss += bendingLoss;
    if (prev && ((x1 == x2 && prev->x != x2) || (y1 == y2 && prev->y != y2))) loss += bendingLoss;
    return loss;
}

NetLoss routeNet(
    const std::vector<std::vector<unsigned int>>& grid, const Circuit::Net& net,
    const unsigned int propagationLoss, const unsigned int crossingLoss, const unsigned int bendingLoss
) {
    auto cmp1 = [](const Node& a, const Node& b) -> bool {
        auto sa = a.cost + a.estimate;
        auto sb = b.cost + b.estimate;
        if (a.lock && b.lock) return true;
        if (a.lock != b.lock) return !a.lock && b.lock;
        if (sa != sb) return sa < sb;
        return a.estimate <= b.estimate;
    };
    std::set<Node, decltype(cmp1)> orderedNodes{cmp1};
    auto cmp2 = [](decltype(orderedNodes.begin()) a, decltype(orderedNodes.begin()) b) -> bool {
        return a->x != b->x ? a->x < b->x : a->y < b->y;
    };
    std::set<decltype(orderedNodes.begin()), decltype(cmp2)> nodes{cmp2};

    auto result = orderedNodes.insert({
        net.x1, net.y1, grid[net.x1][net.y1] * crossingLoss, estimate(
            nullptr, net.x1, net.y1, net.x2, net.y2, propagationLoss, bendingLoss
        )
    });
    nodes.insert(result.first);

    for (;;) { // until target is found
        auto tmpBestNode = *orderedNodes.begin();
        if (nodes.begin() == nodes.end() || tmpBestNode.lock) {
            std::cerr << "Run out of free nodes to explore. This should be impossible." << std::endl;
            exit(1);
        }
        auto itItTmpBestNode = nodes.find(orderedNodes.begin());
        auto itItNextTmpBestNode = nodes.erase(itItTmpBestNode);
        orderedNodes.erase(orderedNodes.begin());
        tmpBestNode.lock = true;
        auto bestNode = orderedNodes.insert(orderedNodes.end(), tmpBestNode);
        nodes.insert(itItNextTmpBestNode, bestNode);
        
        // if (DEBUG > 5) {
        //     for (const auto& node : orderedNodes) {
        //         std::cout << "(" << node.x << ", " << node.y << "): " << node.cost << " + " << node.estimate << " = " << (node.cost + node.estimate)
        //             << (node.lock ? ", locked" : ", free");
        //         if (node.prev) std::cout << " <- (" << node.prev->x << ", " << node.prev->y << ")";
        //         std::cout << std::endl;
        //     }
        //     std::cout << "\n" << std::endl;
        // }

        if (DEBUG > 1) std::cout << "  lock node (" << bestNode->x << ", " << bestNode->y << "), " 
            << "cost: " << bestNode->cost << " + " << bestNode->estimate << " = " 
            << (bestNode->cost + bestNode->estimate) << "" << std::endl;

        if (bestNode->x == net.x2 && bestNode->y == net.y2) { // target found
            Net n;
            n.id = net.id;
            for (const Node* nd = &*bestNode; nd; nd = nd->prev) {
                n.points.push_front({nd->x, nd->y});
            }
            return {n, bestNode->cost};
        }

        for (int i = 0; i < 4; i++) {
            unsigned int x = bestNode->x + upDownLeftRight[i][0];
            unsigned int y = bestNode->y + upDownLeftRight[i][1];
            if (DEBUG > 2) std::cout << "    explore (" << x << ", " << y << "): ";
            if (x >= grid.size() || y >= grid[0].size()) {
                if (DEBUG > 2) std::cout << "out of bound" << std::endl;
                continue;
            }
            if (bestNode->prev && x == bestNode->prev->x && y == bestNode->prev->y) {
                if (DEBUG > 2) std::cout << "previous node" << std::endl;
                continue;
            }

            bool bend = bestNode->prev && x != bestNode->prev->x && y != bestNode->prev->y;
            unsigned int loss = bestNode->cost + propagationLoss;
            if (bend) loss += bendingLoss;
            loss += grid[x][y] * crossingLoss;
            Node newNode {
                x, y, loss, estimate(bestNode->prev, net.x1, net.y1, net.x2, net.y2, propagationLoss, bendingLoss),
                false, &*bestNode
            };
            if (DEBUG > 2) std::cout << "cost: " << newNode.cost << " + " << newNode.estimate << " = " 
                << (newNode.cost + newNode.estimate) << ", ";

            auto sortResult = orderedNodes.insert(newNode);
            if (!sortResult.second) {
                std::cerr << "sortResult.second == false, this should be impossible." << std::endl;
                exit(1);
            }
            auto idResult = nodes.insert(sortResult.first);
            if (idResult.second) { // new node discover
                if (DEBUG > 2) std::cout << "new node" << std::endl;
                continue;
            }
            if (DEBUG > 2) std::cout << "identical to node (" << sortResult.first->x << ", " << sortResult.first->y 
                << "): " << sortResult.first->cost << " + " << sortResult.first->estimate << " = " 
                << (sortResult.first->cost + sortResult.first->estimate) << ", ";
            if (sortResult.first->lock) { // already explored and locked
                orderedNodes.erase(sortResult.first);
                if (DEBUG > 2) std::cout << "already explored and locked" << std::endl;
                continue; 
            }
            // explored but not locked, compare and replace with min one
            if (newNode.cost >= sortResult.first->cost) { // not a better solution
                orderedNodes.erase(sortResult.first);
                if (DEBUG > 2) std::cout << "not a better solution" << std::endl;
                continue; 
            }
            // already explored, not locked, is a better solution
            orderedNodes.erase(*idResult.first);
            auto itItNextNode = nodes.erase(idResult.first);
            nodes.insert(itItNextNode, sortResult.first);
            if (DEBUG > 2) std::cout << "replaced" << std::endl;
        }
    }
}

unsigned int loss(const Net& net, const std::vector<std::vector<unsigned int>>& grid, const Circuit& circuit) {
    unsigned int loss = circuit.propagationLoss * (net.points.size() - 1);
    // if (net.id == 26) std::cout << "net " << net.id << ": propagationLoss = " << circuit.propagationLoss << " * " << net.points.size() - 1 << " = " << loss << std::endl;
    for (size_t i = 0; i < net.points.size(); i++) {
        Point p3 = net.points[i];
        // if (net.id == 26) std::cout << "(" << p3.x << ", " << p3.y << "): \n";
        // if (net.id == 26) std::cout << "loss + crossingLoss * (" << grid[p3.x][p3.y] << " - 1) = ";
        loss += circuit.crossingLoss * (grid[p3.x][p3.y] - 1);
        // if (net.id == 26) std::cout << loss << std::endl;
        if (i < 2) continue;
        Point p1 = net.points[i - 2];
        bool bend = p1.x != p3.x && p1.y != p3.y;
        if (bend) loss += circuit.bendingLoss;
        // if (net.id == 26) if (bend) std::cout << "loss + bendingLoss = " << loss << std::endl;
    }
    return loss;
}

std::vector<Net> route(const Circuit& circuit) {
    std::vector<Net> nets;
    std::vector<std::vector<unsigned int>> grid{circuit.gridX};
    for (auto& col : grid) col.resize(circuit.gridY);

    if (DEBUG) std::cout << "L routing..." << std::endl;
    for (const auto& net : circuit.nets) {
        if (DEBUG > 1) std::cout << "  net " << net.id << ": (" << net.x1 << ", " << net.y1 
            << ") -> (" << net.x2 << ", " << net.y2 << ")" << std::endl;
        Net n;
        n.id = net.id;
        int dx = net.x1 - net.x2; if (dx < 0) dx = -dx;
        int dy = net.y1 - net.y2; if (dy < 0) dy = -dy;
        n.points.resize(dx + dy + 1);
        size_t i = 0;
        for (unsigned int x = net.x1; x != net.x2; net.x1 < net.x2 ? x++ : x--) {
            grid[x][net.y1]++;
            n.points[i++] = {x, net.y1};
        }
        for (unsigned int y = net.y1; y != net.y2; net.y1 < net.y2 ? y++ : y--) {
            grid[net.x2][y]++;
            n.points[i++] = {net.x2, y};
        }
        grid[net.x2][net.y2]++;
        n.points[i] = {net.x2, net.y2};
        nets.push_back(n);
        if (DEBUG > 2) {
            for (const auto& p : n.points) std::cout << "(" << p.x << ", " << p.y << ") ";
            std::cout << std::endl;
        }
    }
        
    // printGrid(grid);
    
    // struct pNetLoss {
    //     Net* net;
    //     unsigned int loss;
    // };
    // std::vector<pNetLoss> netLosses;
    // netLosses.reserve(nets.size());
    // for (auto& net : nets) netLosses.push_back({&net});

    if (DEBUG) std::cout << "\ndetailed routing..." << std::endl;
    for (bool proceed = true; proceed; ) {
        // for (auto& nl : netLosses) nl.loss = loss(*nl.net, grid, circuit);
        // std::sort(netLosses.begin(), netLosses.end(), [](const pNetLoss& a, const pNetLoss& b) {
        //     return a.loss >= b.loss;
        // });

        proceed = false;
        for (auto& net : nets) {
            const auto& ps = net.points;
            Circuit::Net n{net.id, ps.front().x, ps.front().y, ps.back().x, ps.back().y};
            auto oldLoss = loss(net, grid, circuit);
            if (DEBUG) std::cout << "net " << n.id << ": (" << n.x1 << ", " << n.y1 
                << ") -> (" << n.x2 << ", " << n.y2 << "), loss: " << oldLoss << std::endl;
            
            for (const auto& p : ps) grid[p.x][p.y]--;
            auto newNl = routeNet(grid, n, circuit.propagationLoss, circuit.crossingLoss, circuit.bendingLoss);
            if (DEBUG) std::cout << "new route loss: " << newNl.loss << std::endl;
            
            if (newNl.loss < oldLoss) { // found a better solution
                for (const auto& p : newNl.net.points) grid[p.x][p.y]++;
                net = std::move(newNl.net);
                proceed = true;
                if (DEBUG) std::cout << "replace with better route" << std::endl;
            } else if (newNl.loss == oldLoss) { // same
                for (const auto& p : ps) grid[p.x][p.y]++;
            } else {
                std::cerr << "new loss > old loss, this should be impossible." << std::endl;
                DEBUG = 5;
                routeNet(grid, n, circuit.propagationLoss, circuit.crossingLoss, circuit.bendingLoss);
                DEBUG = 1;
                for (const auto& p : ps) grid[p.x][p.y]++;
            }
            // printGrid(grid);
        }
    }

    return nets;
}