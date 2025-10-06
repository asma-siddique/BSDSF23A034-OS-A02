# OS Assignment Report - BSDSF23A034-OS-A02

This report explains the implementation of our simplified ls command in C. 
It discusses the printing logic, different display modes, and design decisions.

## 1. Comparison of Printing Logic

### Across (Horizontal) Printing
- Files are printed row by row, filling each row with as many file names as fit the terminal width.
- Implementation Complexity: Low.
- Pre-calculation: Minimal, only need the maximum filename length for column alignment.

### Down-then-Across (Vertical) Printing
- Files are printed column by column, filling each column top to bottom before moving to the next.
- Implementation Complexity: Higher.
- Pre-calculation: Required to calculate number of rows and columns, and map each file to a position in the grid.

## 2. Strategy for Managing Display Modes (-l, -x, Default)
- Parse command-line arguments and set flags.
- Call the corresponding printing function based on the flags.
- Default: vertical (down-then-across) printing.

