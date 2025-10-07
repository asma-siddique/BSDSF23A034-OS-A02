cat << 'EOF' > REPORT.md
# REPORT.md - LS Assignment

---

## Feature v1.1.0 – Long Listing Format

**Q1: Difference between stat() and lstat()**  
- `stat()` returns info about the target file; `lstat()` returns info about the link itself.  
- In `ls`, use `lstat()` to correctly display symbolic links.

**Q2: Extracting file type and permissions from st_mode**  
- File type: `(st_mode & S_IFMT)` with `S_IFDIR`, `S_IFREG`, etc.  
- Permissions: `(st_mode & S_IRUSR)`, `(st_mode & S_IWUSR)`, `(st_mode & S_IXUSR)`, etc.

---

## Feature v1.2.0 – Column Display (Down Then Across)

**Q1: "Down then across" logic**  
- Files arranged row by row, then across columns: `filename[row + column*num_rows]`.  

**Q2: Purpose of ioctl()**  
- `ioctl()` with `TIOCGWINSZ` gets terminal width.  
- Determines number of columns that fit; fallback width (e.g., 80) is less adaptive.

---

## Feature v1.3.0 – Horizontal Column Display (-x)

**Q1: Vertical vs Horizontal complexity**  
- Vertical requires pre-calculating rows and indexing.  
- Horizontal (-x) simpler: left-to-right printing, wrap when width exceeded.

**Q2: Managing display mode**  
- Use a flag (int or enum) to track mode: default, -l, -x.  
- Call corresponding function based on flag.

---

## Feature v1.4.0 – Alphabetical Sort

**Q1: Why read all entries into memory**  
- Sorting needs access to all filenames.  
- `qsort()` operates on an array; streaming can't sort.  
- Drawback: high memory usage for very large directories.

**Q2: qsort() comparison function**  
- Takes two `const void*`, cast to `char**`.  
- Compare strings with `strcmp()`.  
- Return negative, 0, or positive for sorting order.

---

## Feature v1.5.0 – Colorized Output

**Q1: How ANSI escape codes work**  
- Sequences like `\033[0;32m` set color.  
- Reset with `\033[0m`.  
- Example: `printf("\033[0;32m%s\033[0m\n", filename);` prints green.

**Q2: Determining executables in st_mode**  
- Check `S_IXUSR`, `S_IXGRP`, `S_IXOTH`.  
- Executable if any of these bits are set.

---

## Feature v1.6.0 – Recursive Listing (-R)

**Q1: Base case in recursion**  
- Stops recursion to prevent infinite calls.  
- Base case: no subdirectories or all are `.` or `..`.

**Q2: Why full path construction is essential**  
- Recursive calls need `"parent_dir/subdir"`.  
- Calling just `"subdir"` may fail if CWD differs.

EOF

# Add, commit, and push to GitHub
git add REPORT.md
git commit -m "docs: Added complete REPORT.md with answers for all features"
git push origin main

