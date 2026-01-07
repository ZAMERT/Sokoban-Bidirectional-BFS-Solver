# Sokoban Solver

This is a C++ implementation of a **Sokoban puzzle solver** using **state space search** and **bidirectional BFS**.

----------

## Project Overview

Sokoban is a classic puzzle game where the objective is to push specific boxes to designated target positions on a grid.

This solver focuses on:
- Precise **state representation**.
- Efficient search strategies with limited time and memory restriction.
- Practical performance on non-trivial maps.

----------

## Methods and Algorithms

### State Representation
- Board configuration encoded as a compact state. 
- Player and box positions uniquely define a state.
- States are hashable for fast lookup.

### Search Strategy
- Bidirectional Breadth First Search.
    - Forward search from the initial state.
    - Backward search from all the possible goal state.
    - Reduce search depth significantly.
- Visited-states recorded to avoid redundant exploration.

### Optimization
- Efficient state hashing.
- Early dead-state detection.
- Proper data structure for reducing memory overhead in state storage.

----------

## Usage

Compile: 
``` bash
make
```

Execute: 
- To read the map from a file input.txt. 
``` bash
./sokoban < inputMap.txt
```

- To store the output in a file named output.txt.
``` bash
./sokoban < input.txt > output.txt
```