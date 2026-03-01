# Linear Equation Parser & Solver 

A procedural, zero-dependency C++ engine for parsing, modifying, and solving systems of linear equations via the command line. Built strictly using primitive types, 2D arrays, and independent functions to reinforce algorithmic foundations.

## Features

* **Dynamic Parsing**: Handles implicit coefficients (e.g., `x` implies `1.0x`) and non-standard arrangements (`y + 5 - x = 10z`).
* **Matrix Operations**: Includes built-in calculations for Determinants (`D_value`) and Cramer’s Rule matrices (`D`, `D var`).
* **Equation Arithmetic**: Add, subtract, and dynamically substitute variables across linear constraints.
* **Gaussian Solver**: Uses Partial Pivoting to securely identify exact solutions or flag systems with `No Solution`.

## Installation and Compilation

Since this tool is contained within a single `main.cpp` and utilizes no external packages or OOP hierarchies, compilation is straightforward.

Ensure you have a modern C++ compiler (like `g++`) installed:

```bash
g++ -O3 main.cpp -o solver