# LS Assignment Report – Column Display Feature

## 1. General Logic for "Down Then Across" Columnar Format

- Goal: Display a list of filenames in multiple columns, filling each column **top to bottom** before moving to the next column.
- Why not a single loop?
  - A single loop prints filenames sequentially, which results in a **row-wise** layout, not **column-wise**.
  - To achieve "down then across," we need to calculate **rows and columns** and iterate **row by row**, selecting items from each column.

**Algorithm:**
1. Determine number of columns: `columns = terminal_width / (max_filename_length + padding)`
2. Determine number of rows: `rows = ceil(total_files / columns)`
3. For each row `r` (0 to rows-1):
   - For each column `c` (0 to columns-1):
     - Compute index: `index = c * rows + r`
     - Print `filenames[index]` padded to column width
4. Move to next line after printing each row

This ensures each column is **vertically filled** before moving to the next column.

## 2. Purpose of the `ioctl` System Call

- `ioctl` with `TIOCGWINSZ` is used to **detect the terminal size dynamically** (width and height)
- Purpose in ls column display:
  - Allows the program to calculate **how many columns fit** based on terminal width
  - Ensures filenames are printed in **readable, aligned columns** regardless of terminal size

**Limitations of using a fixed width (e.g., 80 columns):**
1. If terminal is wider than 80 columns, space is wasted
2. If terminal is narrower than 80 columns, filenames may wrap incorrectly, breaking alignment
3. Layout is non-adaptive, reducing usability

