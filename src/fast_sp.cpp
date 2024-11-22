#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include "fast_sp.h"

void FastSP::loadBlocks(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open block file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string outline_str;
        if (iss >> outline_str >> outline.width >> outline.height) {
            if (outline_str != "Outline:") {
                std::cerr << "Error: Invalid block file format. Cannot find Outline!" << std::endl;
                return;
            }
            break;
        } else {
            std::cerr << "Error: Invalid block file format. Expected Outline!" << std::endl;
            continue;
        }
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string num_blocks_str;
        if (iss >> num_blocks_str >> num_blocks) {
            if (num_blocks_str != "NumBlocks:") {
                std::cerr << "Error: Invalid block file format. Cannot find NumBlocks!" << std::endl;
                return;
            }
            break;
        } else {
            std::cerr << "Error: Invalid block file format. Expected NumBlocks!" << std::endl;
            continue;
        }
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string num_terminals_str;
        if (iss >> num_terminals_str >> num_terminals) {
            if (num_terminals_str != "NumTerminals:") {
                std::cerr << "Error: Invalid block file format. Cannot find NumTerminals!" << std::endl;
                return;
            }
            break;
        } else {
            std::cerr << "Error: Invalid block file format. Expected NumTerminals!" << std::endl;
            continue;
        }
    }

    // Load blocks
    // Ex: "Block1 10 20"
    for (int i = 0; i < num_blocks; i++) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name;
            int w, h;
            if (iss >> name >> w >> h) {
                Block* block = new Block(w, h, name);
                blocks.push_back(block);
                blockMap[name] = block;
                break;
            } else {
                std::cerr << "Warning: Skipping invalid block file format. Expected block!" << std::endl;
                std::cerr << "Line: " << line << std::endl;
                continue;
            }
        }
    }

    // Load terminals
    // Ex: "GND terminal 10 20"
    for (int i = 0; i < num_terminals; i++) {
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name, type;
            int x, y;
            if (iss >> name >> type >> x >> y) {
                if (type != "terminal") {
                    std::cerr << "Error: Invalid block file format. Expected terminal type!" << std::endl;
                    return;
                }
                Terminal* terminal = new Terminal(x, y, name);
                terminals.push_back(terminal);
                terminalMap[name] = terminal;
                break;
            } else {
                std::cerr << "Error: Invalid block file format. Expected terminal!" << std::endl;
                std::cerr << "Line: " << line << std::endl;
                continue;
            }
        }
    }

    file.close();
}

void FastSP::loadNets(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open net file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string num_nets_str;
        if (iss >> num_nets_str >> num_nets) {
            if (num_nets_str != "NumNets:") {
                std::cerr << "Error: Invalid net file format. Cannot find NumNets!" << std::endl;
                return;
            }
            break;
        } else {
            std::cerr << "Error: Invalid net file format. Expected NumNets!" << std::endl;
            continue;
        }
    }

    // Load nets
    for (int i = 0; i < num_nets; i++) {
        Net* net = new Net();
        int net_degree;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string net_degree_str;
            if (iss >> net_degree_str >> net_degree) {
                if (net_degree_str != "NetDegree:") {
                    std::cerr << "Error: Invalid net file format. Cannot find NetDegree!" << std::endl;
                    return;
                }
                break;
            } else {
                std::cerr << "Warning: Skipping invalid net file format. Expected NetDegree!" << std::endl;
                std::cerr << "Line: " << line << std::endl;
                continue;
            }
        }
        // Next lines are the terminals in the net, Ex: "T1 T2 T3"
        for (int j = 0; j < net_degree; j++) {
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string terminal_name;
                if (iss >> terminal_name) {
                    bool is_block = blockMap.find(terminal_name) != blockMap.end();
                    bool is_terminal = terminalMap.find(terminal_name) != terminalMap.end();

                    if (is_block) {
                        net->blocks.push_back(blockMap[terminal_name]);
                    } else if (is_terminal) {
                        net->terminals.push_back(terminalMap[terminal_name]);
                    } else {
                        std::cerr << "Warning: Terminal " << terminal_name << " not found! Skipped." << std::endl;
                    }
                    break;
                } else {
                    std::cerr << "Warning: Skipping invalid net file format. Expected terminal!" << std::endl;
                    std::cerr << "Line: " << line << std::endl;
                    continue;
                }
            }
        }

        nets.push_back(net);
    }

    file.close();
}

double FastSP::runFastSP(double runtime) {
    // int init_x, init_y;
    // std::tie(init_x, init_y) = initialize();
    // int best_x = init_x;
    // int best_y = init_y;
    int best_x = evalSequencePair(false);
    int best_y = evalSequencePair(true);
    double best_cost = costFunction(best_x, best_y);
    double overall_best_cost = best_cost;

    double temp = 10000.0;
    double reforge_temp = 1800.0+num_blocks*10;
    double rate = 0.9999;
    int step_per_temp = 10;
    // int max_iter = 100000;
    int best_size = best_x*best_y;
    bool solution_found = best_x <= outline.width && best_y <= outline.height;
    bool overall_solution_found = solution_found;
    SequencePair best_sp;
    best_sp.x = sp.x;
    best_sp.y = sp.y;
    std::vector<bool> best_sp_rotated_blocks(num_blocks, false);
    for (int i = 0; i < num_blocks; i++) {
        best_sp_rotated_blocks[i] = blocks[i]->isRotated();
    }

    int random_depth = 1;
    auto start = std::chrono::high_resolution_clock::now();
    // Simulated annealing
    // 1. No solution found, both x and y are larger than the outline
    //   - Accept action base on the area
    // 2. No solution found, one of x or y is larger than the outline
    //   - Accept action base on the other dimension
    // 3. Solution found
    //   - Accept action base on the cost function
    int i = 0;
    int freezed_count = 0;
    int finish_count = 0;

    while (1) {
        i++;
        for (int j = 0; j < step_per_temp; j++) {
            std::vector<LastMove> last_moves = randomAction(random_depth);
            int len_x = evalSequencePair(false);
            int len_y = evalSequencePair(true);
            double cost = DBL_MAX;
            int size = len_x*len_y;
            bool accept = false;
            bool accept_uphill = false;

            if (!solution_found) {
                if (len_x <= outline.width && len_y <= outline.height) {
                    accept = true;
                    solution_found = true;
                    cost = costFunction(len_x, len_y);
                } else if (len_x <= outline.width) {
                    if (len_y <= best_y) {
                        accept = true;
                    }
                } else if (len_y <= outline.height) {
                    if (len_x <= best_x) {
                        accept = true;
                    }
                } else {
                    if (size < best_size) {
                        accept = true;
                    }
                }
            } else {
                // Only calculate cost when solution is found
                cost = costFunction(len_x, len_y);
                if (cost < best_cost) {
                    accept = true;
                }

                if (len_x > outline.width || len_y > outline.height) {
                    accept = false;
                }
            }

            if (!accept) {
                double prob;
                if (solution_found) {
                    prob = exp((best_cost - cost) / temp);
                } else {
                    prob = exp((best_size - size) / temp);
                }
                std::uniform_real_distribution<double> dist(0.0, 1.0);
                if (dist(rng) < prob) {
                    accept_uphill = true;
                    if (len_x > outline.width || len_y > outline.height) {
                        solution_found = false;
                    }
                } else {
                    accept_uphill = false;
                    undoAction(last_moves);
                }
            }

            if (accept || accept_uphill) {
                best_cost = cost;
                best_size = size;
                best_x = len_x;
                best_y = len_y;
                if ((cost < overall_best_cost || (!overall_solution_found)) && best_x <= outline.width && best_y <= outline.height) {
                    overall_solution_found = true;
                    overall_best_cost = cost;
                    best_sp = sp;
                    for (int i = 0; i < num_blocks; i++) {
                        best_sp_rotated_blocks[i] = blocks[i]->isRotated();
                    }
                    freezed_count = 0;
                    finish_count = 0;
                }
            } else {
                freezed_count++;
            }
        }

        if (i % 10000 == 0) {
            // std::cout << "Iteration: " << i << std::endl;
            // std::cout << "Overall best cost: " << overall_best_cost << std::endl;
            // std::cout << "Best cost: " << best_cost << std::endl;
            // std::cout << "Best size: " << best_size << std::endl;
            // std::cout << "Best X: " << best_x << std::endl;
            // std::cout << "Best Y: " << best_y << std::endl;
            // std::cout << "Solution found: " << solution_found << std::endl;
            // std::cout << "Temp: " << temp << std::endl;
        }

        if (temp < 1e1) {
            temp = reforge_temp;
            // std::cout << "==========" << std::endl;
            // std::cout << "Reforge!" << std::endl;
            // std::cout << "==========" << std::endl;
            randomAction(num_blocks);
            if (overall_solution_found) {                
                if (finish_count == 40) {
                    break;
                }
                if (finish_count % 3 == 0) {
                    // Revert to the best solution
                    sp = best_sp;
                    for (int i = 0; i < num_blocks; i++) {
                        if (blocks[i]->isRotated() != best_sp_rotated_blocks[i]) {
                            blocks[i]->rotate90();
                        }
                    }
                }
                finish_count += 1;
                freezed_count = 0;
            }
        };

        double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count();
        if (elapsed > runtime) {
            break;
        }

        temp *= rate;
    }

    sp = best_sp;
    for (int i = 0; i < num_blocks; i++) {
        if (blocks[i]->isRotated() != best_sp_rotated_blocks[i]) {
            blocks[i]->rotate90();
        }
    }
    return overall_solution_found ? overall_best_cost : DBL_MAX;
}

std::pair<int, int> FastSP::initialize() {
    // Initialize pos_x and pos_y
    pos_x = std::vector<int>(num_blocks+1, 0);
    pos_y = std::vector<int>(num_blocks+1, 0);

    // Initialize sequence pair
    sp.x = std::vector<int>(num_blocks+1, 0);
    sp.y = std::vector<int>(num_blocks+1, 0);
    for (int i = 1; i < num_blocks+1; i++) {
        sp.x[i] = i;
        sp.y[i] = i;
    }

    // Initialize random number generator
    std::random_device rd;
    seed = rd();
    rng = std::mt19937(seed);

    // Randomly shuffle the sequence pair
    // Pick the best one out of 1000 random shuffles
    double best_cost = 1e9;
    int best_x = 0;
    int best_y = 0;
    SequencePair best_sp;
    for (int i = 0; i < 4000; i++) {
        std::shuffle(sp.x.begin()+1, sp.x.end(), rng);
        std::shuffle(sp.y.begin()+1, sp.y.end(), rng);
        int len_x = evalSequencePair(false);
        int len_y = evalSequencePair(true);
        double cost = costFunction(len_x, len_y);
        if (cost < best_cost) {
            best_cost = cost;
            best_x = len_x;
            best_y = len_y;
            best_sp = sp;
        }
    }
    // std::cout << "Initialization done!" << std::endl;
    // std::cout << "Best cost: " << best_cost << std::endl;
    // std::cout << "Best size: " << best_size << std::endl;
    // std::cout << "Best X: " << best_x << std::endl;
    // std::cout << "Best Y: " << best_y << std::endl;
    sp = best_sp;
    return std::make_pair(best_x, best_y);
}

std::vector<LastMove> FastSP::randomAction(int depth) {
    std::uniform_int_distribution<int> index_dist(1, num_blocks);
    std::uniform_int_distribution<int> action_dist(0, 3);
    std::vector<LastMove> last_moves(depth);
    for (int i = 0; i < depth; i++) {
        int j = index_dist(rng);
        int k = index_dist(rng);
        int action = action_dist(rng);
        if (action == 0) {
            swapSingle(false, j, k);
        } else if (action == 1) {
            swapSingle(true, j, k);
        } else if (action == 2) {
            swapBoth(j, k);
        } else {
            rotate90(j);
        }
        last_moves[i] = {j, k, action};
    }
    return last_moves;
}

void FastSP::swapSingle(bool do_y_or_x, int i, int j) {
    if (do_y_or_x) {
        std::swap(sp.y[i], sp.y[j]);
    } else {
        std::swap(sp.x[i], sp.x[j]);
    }
}

void FastSP::swapBoth(int i, int j) {
    std::swap(sp.x[i], sp.x[j]);
    std::swap(sp.y[i], sp.y[j]);
}

void FastSP::rotate90(int i) {
    blocks[i-1]->rotate90();
}

void FastSP::undoAction(std::vector<LastMove>& last_moves) {
    // Reverse the last moves
    for (int i = last_moves.size()-1; i >= 0; i--) {
        int j = last_moves[i].i;
        int k = last_moves[i].j;
        int action = last_moves[i].action;
        if (action == 0) {
            swapSingle(false, j, k);
        } else if (action == 1) {
            swapSingle(true, j, k);
        } else if (action == 2) {
            swapBoth(j, k);
        } else {
            rotate90(j);
        }
    }
}

//================================================================
// Psuedo code for FastSP Algorithm:
//================================================================
// ```
// Algo FastSP(X, Y)
// 1    Initialize MATCH
// 2    Initialize H
// 3    Initialize BUCKL with BUCKL[0] = 0
// 4    for i = 1 to n do
// 5        b = X[i]
// 6        p = MATCH[b].y
// 7        insert p to H and BUCKL
// 8        POS[p] = BUCKL[predacessor of p in H]
// 9        BUCKL[p] = POS[p] + weignt(b)
// 10       Discard the sucessor of p in H and BUCKL whose value <= BUCKL[p]
// 11   return BUCKL[index_max]
// ```
//================================================================
// - X: Sequence pair X, permutation of {l ... n}
// - Y: Sequence pair Y, permutation of {l ... n}
// - n: Number of blocks
// - POS[n]: X or Y coordinate of the block n
// - MATCH[b].x: Indice of block b in X
// - MATCH[b].y: Indice of block b in Y
//   - i.e. MATCH[b].x = i and MATCH[b].y = j if b = X[i] = Y[j]
// - H: Host tree, implemented in FastBitSetTree
// - BUCKL: Buckets for the host tree
//   - BUCKL[p] records the length of the longest common subsequence.
int FastSP::evalSequencePair(bool do_y_or_x) {
    if (do_y_or_x) {
        reverse(sp.x.begin()+1, sp.x.end());
    }
    // Initialize MATCH
    // Pad with 0 to make 1-indexed
    // 0 is the starting point `s`
    std::vector<int> match(num_blocks+1);
    match[0] = 0;
    for (int i = 1; i < num_blocks+1; i++) {
        // if (do_y_or_x) {
        //     match[sp.x[i]] = i;
        // } else {
            match[sp.y[i]] = i;
        // }
    }
    // Initialize H
    FastBitSetTree host(num_blocks+1);
    host.insert(0);
    // Initialize BUCKL with BUCKL[0] = 0
    // -1 means empty
    std::vector<int> BUCKL(num_blocks+1, -1);
    BUCKL[0] = 0;
    for (int i = 1; i < num_blocks+1; i++) {
        int b = sp.x[i];
        int p = match[b];   // P is the position of b in Y
        host.insert(p);
        int pred = host.predecessor(p);
        int pos = BUCKL[pred];
        if (do_y_or_x) {
            // pos_y[p] = pos;
            BUCKL[p] = pos + blocks[b-1]->getHeight();
            blocks[b-1]->setCoordY(pos);
        } else {
            // pos_x[p] = pos;
            BUCKL[p] = pos + blocks[b-1]->getWidth();
            blocks[b-1]->setCoordX(pos);
        }
        int succ = host.successor(p);
        while (succ != -1) {
            int pre_succ = succ;
            succ = host.successor(succ);
            if (BUCKL[pre_succ] <= BUCKL[p]) {
                host.remove(pre_succ);
                BUCKL[pre_succ] = -1;
            }
        }
    }
    if (do_y_or_x) {
        reverse(sp.x.begin()+1, sp.x.end());
    }
    
    return BUCKL[host.maximum()];
}

int FastSP::netHPWL(Net* net) {
    if (net->blocks.empty() && net->terminals.empty()) {
        return 0;
    }

    int xmin = INT_MAX;
    int xmax = INT_MIN;
    int ymin = INT_MAX;
    int ymax = INT_MIN;

    for (auto block : net->blocks) {
        Point* coord = block->getCoord();
        int x = coord->x + block->getWidth() / 2;
        int y = coord->y + block->getHeight() / 2;
        xmin = std::min(xmin, x);
        xmax = std::max(xmax, x);
        ymin = std::min(ymin, y);
        ymax = std::max(ymax, y);
    }

    for (auto terminal : net->terminals) {
        Point* coord = terminal->getCoord();
        xmin = std::min(xmin, coord->x);
        xmax = std::max(xmax, coord->x);
        ymin = std::min(ymin, coord->y);
        ymax = std::max(ymax, coord->y);
    }

    return (xmax - xmin) + (ymax - ymin);
}

int FastSP::totalHPWL() {
    int total = 0;
    for (auto net : nets) {
        total += netHPWL(net);
    }
    return total;
}

SequencePair FastSP::deepCopySP(SequencePair& sp) {
    return sp;
}

double FastSP::costFunction(int len_x, int len_y) {
    double cost = alpha * (len_x * len_y) + (1 - alpha) * totalHPWL();
    return cost;
}

void FastSP::outputBlocks(std::string filename = "output_blocks.txt") {
    evalSequencePair(false);
    evalSequencePair(true);
    std::ofstream file(filename);
    for (int i = 1; i < num_blocks+1; i++) {
        Block* block = blocks[i-1];
        file << block->getName() << " " << block->getCoordX() << " " << block->getCoordY() << " " << block->getWidth() << " " << block->getHeight() << std::endl;
    }
    file.close();
}

void FastSP::outputHPWL(std::string filename) {
    std::ofstream file(filename);
    for (int i = 0; i < num_nets; i++) {
        Net* net = nets[i];
        int hpwl = netHPWL(net);
        file << "Net: " << i << " " << hpwl << std::endl;
    }
    file.close();
}

void FastSP::outputDump(std::string filename, double runtime) {
    int len_x = evalSequencePair(false);
    int len_y = evalSequencePair(true);
    std::ofstream file(filename);
    int total_wirelength = totalHPWL();
    int cost = costFunction(len_x, len_y);
    file << (int)cost << std::endl;
    file << total_wirelength << std::endl;
    file << len_x*len_y << std::endl;
    file << len_x << " " << len_y << std::endl;
    file << runtime << std::endl;
    std::cout << "Best cost: " << cost << std::endl;
    std::cout << "Best size: " << len_x*len_y << std::endl;
    std::cout << "Best X: " << len_x << std::endl;
    std::cout << "Best Y: " << len_y << std::endl;
    for (int i = 1; i < num_blocks+1; i++) {
        Block* block = blocks[i-1];
        file << block->getName() << " " << block->getCoordX() << " " << block->getCoordY() << " " << block->getCoordX()+block->getWidth() << " " << block->getCoordY()+block->getHeight() << std::endl;
    }
    // outputBlocks("output_blocks.txt");
    // outputHPWL("output_hpwl.txt");

    // Print all blocks
    // std::cout << "NumBlocks: " << num_blocks << std::endl;
    // for (int i = 1; i < num_blocks+1; i++) {
    //     Block* block = blocks[i-1];
    //     std::cout << block->getName() << ": " << block->getCoordX() << " " << block->getCoordY() << " " << block->getCoordX()+block->getWidth() << " " << block->getCoordY()+block->getHeight() << std::endl;
    // }
    file.close();
}

void FastSP::copySolution(FastSP* other) {
    sp = other->sp;
    for (int i = 0; i < num_blocks; i++) {
        blocks[i]->setCoordX(other->blocks[i]->getCoordX());
        blocks[i]->setCoordY(other->blocks[i]->getCoordY());
        if (blocks[i]->isRotated() != other->blocks[i]->isRotated()) {
            blocks[i]->rotate90();
        }
    }
}