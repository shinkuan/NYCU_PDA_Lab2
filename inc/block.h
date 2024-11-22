//############################################################################
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//   Block and Terminal Classes Implementation Header File
//   
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//   File Name   : block.h
//   Release Version : V1.0
//   Description : 
//      This header file defines the classes `Terminal`, `Block`, and `Net` 
//      which are essential for managing geometric coordinates of terminals 
//      and blocks in Fast SP algorithm. It includes basic operations such as 
//      setting and retrieving the coordinates, dimensions, and names of 
//      these entities. The `Net` class maintains a collection of `Terminal` 
//      and `Block` objects.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//   Key Features:
//   1. `Terminal` Class:
//        - Represents a point in a 2D grid with x and y coordinates and a name.
//        - Provides setter and getter methods for coordinates and name.
//   2. `Block` Class:
//        - Represents a rectangular block with x, y coordinates, width, height, and name.
//        - Provides setter and getter methods for the block's properties.
//   3. `Net` Class:
//        - Manages collections of `Terminal` and `Block` objects for modeling
//          connections between these elements.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//   Author          : shinkuan
//   Creation Date   : 2024-10-06
//   Last Modified   : 2024-10-06
//   Compiler        : g++/clang++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//   Usage Example:
//   #include "block.h"
//
//   Terminal t1(0, 0, "T1");    // Initialize terminal at (0, 0) with name "T1"
//   Block b1(10, 20, "B1");     // Create a block with width 10, height 20, named "B1"
//   Net net;                    // Initialize a network of blocks and terminals
//   net.terminals.push_back(&t1);
//   net.blocks.push_back(&b1);
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//   License: 
//
//############################################################################

#ifndef _BLOCK_H_
#define _BLOCK_H_

#include <string>
#include <vector>

struct Point {
    int x;
    int y;
};

class Terminal {
private:
    //================================================================
    // Member Variables
    //================================================================
    Point coord;
    std::string name;

public:
    //================================================================
    // Constructors and Destructors
    //================================================================
    Terminal() : 
        coord({0, 0}), name("") {}

    Terminal(int x, int y, std::string n) : 
        coord({x, y}), name(n) {}

    ~Terminal() {}

    //================================================================
    // Getters and Setters
    //================================================================
    Point* getCoord()           { return &coord;}
    std::string getName()       { return name;  }

    void setCoord(Point p)      { coord = p;    }
    void setName(std::string n) { name = n;     }
    void setCoordX(int x)       { coord.x = x;  }
    void setCoordY(int y)       { coord.y = y;  }
    void setCoord(int x, int y) {
        coord.x = x;
        coord.y = y;
    }
};

class Block {
private:
    //================================================================
    // Member Variables
    //================================================================
    Point coord;
    int width;
    int height;
    std::string name;
    bool rotated;

public:
    //================================================================
    // Constructors and Destructors
    //================================================================
    Block() : 
        coord({0, 0}), width(0), height(0), name(""), rotated(false) {}

    Block(int w, int h, std::string n) : 
        coord({0, 0}), width(w), height(h), name(n), rotated(false) {}

    Block(int x, int y, int w, int h, std::string n) : 
        coord({x, y}), width(w), height(h), name(n), rotated(false) {}

    ~Block() {}

    //================================================================
    // Getters and Setters
    //================================================================
    Point* getCoord()           { return &coord;  }
    Point getMiddle()           { return {coord.x + width / 2, coord.y + height / 2}; }
    int getCoordX()             { return coord.x; }
    int getCoordY()             { return coord.y; }
    int getWidth()              { return width;   }
    int getHeight()             { return height;  }
    std::string getName()       { return name;    }
    bool isRotated()            { return rotated; }

    void setCoord(Point p)      { coord = p;      }
    void setWidth(int w)        { width = w;      }
    void setHeight(int h)       { height = h;     }
    void setName(std::string n) { name = n;       }
    void setCoordX(int x)       { coord.x = x;    }
    void setCoordY(int y)       { coord.y = y;    }
    void setCoord(int x, int y) { coord = {x, y}; }
    void rotate90()   { std::swap(width, height); rotated = !rotated; }
};

class Net {
public:
    //================================================================
    // Member Variables
    //================================================================
    std::vector<Terminal*> terminals;
    std::vector<Block*> blocks;

    //================================================================
    // Constructors and Destructors
    //================================================================
    Net() {}
    ~Net() {}
};
#endif // _BLOCK_H_