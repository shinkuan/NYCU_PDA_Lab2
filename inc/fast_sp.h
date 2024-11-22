

#ifndef _FAST_SP_H_
#define _FAST_SP_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include "bst.h"
#include "block.h"


struct Outline {
    int width;
    int height;
};

struct SequencePair {
    std::vector<int> x;
    std::vector<int> y;
};

struct LastMove {
    int i;
    int j;
    int action;
};

struct Indice {
    int x;
    int y;
};

class FastSP {
private:
    //================================================================
    // Member Variables
    //================================================================
    double alpha;
    Outline outline;
    std::vector<Block*> blocks;
    std::vector<Terminal*> terminals;
    std::vector<Net*> nets;
    std::unordered_map<std::string, Block*> blockMap;
    std::unordered_map<std::string, Terminal*> terminalMap;
    SequencePair sp;
    int num_terminals;
    int num_blocks;
    int num_nets;
    std::vector<int> pos_x;
    std::vector<int> pos_y;

    int seed = 1337;
    std::mt19937 rng;

public:
    //================================================================
    // Constructors and Destructors
    //================================================================
    FastSP() : alpha(1.0), outline({0, 0}) {}
    FastSP(double a) : alpha(a), outline({0, 0}) {}
    FastSP(double a, std::string block_file, std::string net_file) : alpha(a), outline({0, 0}) {
        loadBlocks(block_file);
        loadNets(net_file);
    }
    ~FastSP() {
        for (int i = 0; i < num_blocks; i++) {
            delete blocks[i];
        }
        for (int i = 0; i < num_terminals; i++) {
            delete terminals[i];
        }
        for (int i = 0; i < num_nets; i++) {
            delete nets[i];
        }
    }

    //================================================================
    // Getters and Setters
    //================================================================
    double getAlpha() { return alpha; }
    Outline getOutline() { return outline; }
    void setAlpha(double a) { alpha = a; }
    void setOutline(Outline o) { outline = o; }

    //================================================================
    // Loaders
    //================================================================
    void loadBlocks(std::string filename);
    void loadNets(std::string filename);

    //================================================================
    // FastSP Algorithm
    //================================================================
    double runFastSP(double runtime);
    std::pair<int, int> initialize();
    std::vector<LastMove> randomAction(int depth);
    void swapSingle(bool do_y_or_x, int i, int j);
    void swapBoth(int i, int j);
    void rotate90(int i);
    void undoAction(std::vector<LastMove>& last_moves);
    int evalSequencePair(bool do_y_or_x);
    int netHPWL(Net* net);
    int totalHPWL();

    //================================================================
    // Utility Functions
    //================================================================
    SequencePair deepCopySP(SequencePair& sp);
    double costFunction(int len_x, int len_y);
    void outputBlocks(std::string filename);
    void outputHPWL(std::string filename);
    void outputDump(std::string filename, double runtime);
    void copySolution(FastSP* other);
};



#endif // _FAST_SP_H_