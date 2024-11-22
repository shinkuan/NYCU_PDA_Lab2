
# NYCU_PDA_Lab2 
This repository contains the implementation of **Lab 2: Fixed Outline Floorplanning**  from the **Physical Design Automation (PDA)**  course at National Yang Ming Chiao Tung University (NYCU). This programming assignment asks you to write a fixed-outline floorplanner to handle hard macros. Given a set of rectangular macros and nets, the floorplanner places all macros within a rectangular chip without any overlaps. We assume that the lower-left corner of this chip is the origin (0, 0), and no space (channel) is needed between two different macros. The objective is to minimize the area of the chip bounding box and the total net wire length.

---


## Table of Contents 
 
1. [Introduction](https://chatgpt.com/c/674018e5-e898-8001-94b9-f3dee2ec3f9e#introduction)
 
2. [Features](https://chatgpt.com/c/674018e5-e898-8001-94b9-f3dee2ec3f9e#features)
 
3. [Usage](https://chatgpt.com/c/674018e5-e898-8001-94b9-f3dee2ec3f9e#usage)
 
4. [Visualization](https://chatgpt.com/c/674018e5-e898-8001-94b9-f3dee2ec3f9e#visualization)
 
5. [References](https://chatgpt.com/c/674018e5-e898-8001-94b9-f3dee2ec3f9e#references)


---


## Introduction 

The objective of this lab is to place all blocks (hard macros) into a fixed rectangular chip outline while minimizing:
 
- The **bounding-box area**  of the floorplan.
 
- The **total wirelength**  of the nets (calculated using Half-Perimeter Wire Length, HPWL).

The final cost function is defined as:

$$
 \text{Cost} = \alpha A + (1 - \alpha) W 
$$

Where:
 
- $$\alpha$$: A user-defined weight between area and wirelength (0 ≤ α ≤ 1).
 
- $$A$$: Bounding-box area.
 
- $$W$$: Total HPWL of all nets.


---


## Features 
 
1. **Fast and Efficient Placement** : Utilizes an optimized **Fast Bit-Set Tree**  data structure for near $$O(1)$$ operations.
 
2. **Input and Output Management** : 
  - Input files are located in the `./input` directory.
 
  - Output files are stored in the `./output` directory.
 
3. **Comprehensive Testing** : 
  - Scripts (`./run`, `./run.bat`, `./run_verifier`) are provided to automate the testing process.
 
4. **Visualization** : 
  - A Python-based tool is available to visualize floorplan results (`draw_block.py`).


---


## Usage 

### Compilation 
Compile the program using the provided `Makefile`:

```bash
make
```
This will generate an executable named `Lab2`.
### Execution 

Run the program with the following command:


```bash
./Lab2 [α value] [input.block name] [input.net name] [output name]
```

#### Example: 


```bash
./Lab2 0.5 ./input/example.block ./input/example.nets ./output/result.rpt
```

### Testing 

To run all test cases with provided scripts:
 
- Linux:

```bash
./run
```
 
- Windows:

```cmd
run.bat
```

You can verify results using the verifier script:


```bash
./run_verifier
```


---


## Visualization 

Visualize the floorplan using the provided Python script:


```bash
python3 draw_block.py [layout_file] [output_image]
```

#### Example: 


```bash
python3 draw_block.py ./output/layout.txt ./visualize/layout.png
```
Visualized results are stored in the `./visualize` directory.

---


## Advanced Data Structure: Fast Bit-Set Tree 
This implementation improves upon the original sequence pair algorithm by introducing the **Fast Bit-Set Tree** , a highly efficient static set data structure designed for: 
- **Fast operations**  such as insertion, deletion, and successor/predecessor searches.
 
- Block-level efficiency using **bitwise operations**  (e.g., `__builtin_ffsll`, `__builtin_clzll`) for $$O(1)$$ in-block queries.
The result is a structure that achieves near $$O(1)$$ complexity for tree operations, outperforming traditional $$O(\log\log n)$$ methods.

---


## References 

This implementation is based on the following research paper:
 
- **Xiaoping Tang and D. F. Wong** , *"FAST-SP: a fast algorithm for block placement based on sequence pair"*, Proceedings of the ASP-DAC 2001.
Enhancements to the original algorithm were made by replacing the $$O(\log\log n)$$ tree structure with the **Fast Bit-Set Tree** , optimizing both performance and memory usage.