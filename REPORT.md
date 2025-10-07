REPORT.md Answers
Feature v1.1.0 – Long Listing Format

Q1: Difference between stat() and lstat()
A:

stat() retrieves information about a file. If the file is a symbolic link, it returns info about the target file, not the link itself.

lstat() retrieves info about the link itself if the file is a symbolic link.

In ls, lstat() is preferred to correctly display symbolic links as links.

Q2: How to extract file type and permissions from st_mode
A:

Use macros like S_IFDIR, S_IFREG with st_mode & S_IFMT to determine file type.

Use permission macros: S_IRUSR, S_IWUSR, S_IXUSR, etc., with bitwise AND (&) to check read/write/execute permissions for user, group, and others.

Feature v1.2.0 – Column Display (Down Then Across)

Q1: Explain "down then across" printing logic
A:

Files are arranged in rows first, then columns.

Iterate row by row, not by simple index, using formula:
filename[row + column*num_rows]

This ensures vertical alignment across columns.

Q2: Purpose of ioctl() and terminal width detection
A:

ioctl() with TIOCGWINSZ gets the terminal width.

Terminal width is needed to calculate how many columns can fit.

Without it, layout could overflow or look unaligned; fallback width (like 80) is less adaptive.

Feature v1.3.0 – Horizontal Column Display (-x)

Q1: Compare vertical vs horizontal logic complexity
A:

Vertical ("down then across") requires pre-calculating rows, iterating row-wise, and careful indexing.

Horizontal (-x) is simpler: just print left to right, wrap line when width is exceeded.

Q2: How display mode is managed
A:

Use a flag (int or enum) to track display mode: default, -l, or -x.

Based on flag, call the corresponding display function in do_ls().

Feature v1.4.0 – Alphabetical Sort

Q1: Why read all directory entries into memory?
A:

Sorting requires access to all filenames simultaneously.

qsort() operates on an array; streaming entries one by one cannot be sorted.

Drawback: high memory usage for directories with millions of files.

Q2: How qsort() comparison function works
A:

Takes two const void* pointers.

Cast to char** and use strcmp() to compare strings.

Returns negative, zero, or positive for sorting order.

Feature v1.5.0 – Colorized Output

Q1: How ANSI escape codes work
A:

Terminal interprets sequences like \033[0;32m to set color/style.

After printing text, reset to default with \033[0m.

Example: green text → printf("\033[0;32m%s\033[0m\n", filename);

Q2: Which st_mode bits determine executables
A:

Check S_IXUSR, S_IXGRP, S_IXOTH for owner, group, and others.

File is executable if any of these bits are set.

Feature v1.6.0 – Recursive Listing (-R)

Q1: What is a base case in recursion?
A:

Base case stops recursion to prevent infinite calls.

In do_ls(), the base case is when a directory has no subdirectories or all subdirectories are . or ...

Q2: Why full path construction is essential
A:

Recursive calls must know the complete path: "parent_dir/subdir".

Calling just do_ls("subdir") would fail if the current working directory is different.
