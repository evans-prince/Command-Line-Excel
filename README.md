# Command-Line Excel: Spreadsheet Project

**Author:** Prince\
**Start Date:** 01-Feb-2025

---

## 📌 Overview

This project is a command-line spreadsheet program developed in C, supporting integer-only cell values, formulas, and efficient recalculation using a Directed Acyclic Graph (DAG). It mimics core functionalities of Excel such as cell assignment, formula evaluation, and dependency tracking, all within a terminal interface.

---

## ✨ Features

- 📊 **Spreadsheet Grid**: Supports sizes from 1x1 up to 999x18278 (A1 to ZZZ999).
- 🔗 **Formula Evaluation**: Supports arithmetic operations and range-based functions (e.g., `SUM(A1:A5)`).
- 🔄 **Efficient Recalculation**: Uses topological sorting and a calculation chain for updating only necessary cells.
- ♻️ **Dependency Management**: Tracks dependencies and dependents for each cell using CellRange structs.
- 🚫 **Error Handling**: Detects circular references, invalid inputs, and handles edge cases like division by zero.
- 🧪 **Testing Suite**: Modular unit tests for formula parsing, input handling, and recalculation.
- 🧱 **Modular Design**: Divided into reusable modules for better maintainability and scalability.

---

## 🛠️ Design Structure

### Spreadsheet Core

- `cell** grid`: 2D array for cells.
- `calculation_chain[]`: Optimized recalculation order.
- `CommandStatus`: Stores status and execution time of last command.

### Input Handling

- Tokenizes and classifies inputs using enums.
- Validates syntax for cell assignments, formulas, function calls, and scroll commands.

### Recalculation Engine

- Dirty flag mechanism for tracking updates.
- Topological sorting to determine update order.
- Selective recalculation based on dependencies.

### Error Handling

- Invalid syntax detection.
- Circular reference prevention.
- Graceful handling of undefined operations (e.g., `A1=10/0`).

---

## 🧩 Directory Structure

```plaintext
.
├── include/               # Header files
│   ├── spreadsheet.h
│   ├── recalculation.h
│   ├── input_handler.h
│   └── ...
├── src/                   # Source files
│   ├── spreadsheet.c
│   ├── recalculation.c
│   ├── input_handler.c
│   └── ...
├── lib/                   # Utility functions
│   └── utils.c
├── tests/                 # Unit tests
│   ├── test_input.c
│   ├── test_formula.c
│   └── ...
├── Makefile               # Build automation
├── script.sh              # Script to run and test the program
└── report/                # LaTeX report files
    └── report.pdf
```

---
## How to Run 🚀

### Prerequisites

A Linux or macOS system

GCC (GNU Compiler Collection)

Steps to Build & Run
```
# Clone the repository
$ git clone https://github.com/Cyanide-03/Command-Line-Excel.git
$ cd Command-Line-Excel

# Build the project
$ make

# Run the program
$ ./target/release/spreadsheet 999 18278

# NOTE : You can also run the program using 
$ ./script.sh
# If the permission is not given for script.sh use 
$ chomod +x script.sh
```

Optional
```
# Run provided tests
$ make test

# Clean 
$ make clean

# Use script for custom commands

./script.sh
```
---

## 🔗 Project Links

- 📁 **GitHub Repository**: [Command-Line Excel](https://github.com/Cyanide-03/Command-Line-Excel)
- 📽️ **Demo Video**: [Watch Demo](https://csciitd-my.sharepoint.com/\:f:/r/personal/ph1221248iitdacin/Documents/cop290−demo−video?csf=1\&web=1\&e=5IphAe)

---

## 📣 Contributions

This project was collaboratively developed by:

- 👨‍💻 Prince
- 👨‍💻 Aditya

---

## 📞 Contact

If you'd like to know more or contribute, feel free to open an issue or contact us via GitHub!

