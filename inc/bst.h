#include <bits/stdc++.h>

class FastBitSetTree {
private:
    std::vector<uint64_t> bits;
    int u;
    
public:
    FastBitSetTree(int upper_bound) : u(upper_bound) {
        int num_blocks = (u + 64) / 64;
        bits.assign(num_blocks, 0);
    }
    
    void insert(int value) {
        if (value < 0 || value > u) return;
        int block = value / 64;
        int pos = value % 64;
        bits[block] |= (1ULL << pos);
    }
    
    void remove(int value) {
        if (value < 0 || value > u) return;
        int block = value / 64;
        int pos = value % 64;
        bits[block] &= ~(1ULL << pos);
    }
    
    int successor(int value) const {
        if (value >= u) return -1;
        size_t block = (value+1) / 64;
        int pos = (value+1) % 64;
        
        // Mask out the bits <= value
        uint64_t current = bits[block] & (~0ULL << pos);
        
        if (current != 0) {
            int offset = __builtin_ffsll(current) - 1;
            return block * 64 + offset;
        }
        
        // Check subsequent blocks
        for (++block; block < bits.size(); ++block) {
            if (bits[block] != 0) {
                int offset = __builtin_ffsll(bits[block]) - 1;
                return block * 64 + offset;
            }
        }
        return -1; // No successor
    }
    
    int predecessor(int value) const {
        if (value <= 0) return -1;
        int block = (value) / 64;
        int pos = (value) % 64;
        
        // Mask out the bits >= value
        uint64_t current = bits[block] & (((1ULL << pos) - 1));
        if (current != 0) {
            int offset = 63 - __builtin_clzll(current);
            return block * 64 + offset;
        }
        
        // Check previous blocks
        for (--block; block >= 0; --block) {
            if (bits[block] != 0) {
                int offset = 63 - __builtin_clzll(bits[block]);
                return block * 64 + offset;
            }
        }
        return -1; // No predecessor
    }

    int minimum() const {
        for(size_t block = 0; block < bits.size(); ++block) {
            if(bits[block] != 0) {
                int offset = __builtin_ffsll(bits[block]) - 1;
                if(offset >= 0)
                    return block * 64 + offset;
            }
        }
        return -1;
    }

    int maximum() const {
        for(int block = bits.size() - 1; block >= 0; --block) {
            if(bits[block] != 0) {
                int leading_zeros = __builtin_clzll(bits[block]);
                if(leading_zeros < 64) {
                    int offset = 63 - leading_zeros;
                    return block * 64 + offset;
                }
            }
        }
        return -1;
    }
};