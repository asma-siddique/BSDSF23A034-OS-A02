# OS Assignment Report

## 1. Difference between stat() and lstat()

The crucial difference between stat() and lstat() is how they handle **symbolic links**:

- stat(path, &statbuf)  
  Follows the symbolic link and returns information about the **target file**.
  
- lstat(path, &statbuf)  
  Returns information about the **link itself**, not the target file.

**In the context of ls:**  
- Use lstat() when you want to list symbolic links themselves (e.g., show that a file is a symlink and its permissions) rather than the file they point to.

---

## 2. Using st_mode to determine file type and permissions

The st_mode field in struct stat contains **both file type and permission bits**.

### 2.1 Extracting file type

Use bitwise AND & with macros like S_IFDIR, S_IFREG, etc.:

```c
if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
    printf("It's a directory\n");
} else if ((statbuf.st_mode & S_IFMT) == S_IFREG) {
    printf("It's a regular file\n");
}
```

Here, S_IFMT is a **mask for the file type bits**.

### 2.2 Extracting permission bits

Use bitwise AND & with macros like S_IRUSR, S_IWUSR, S_IXUSR (read, write, execute permissions for the user):

```c
if (statbuf.st_mode & S_IRUSR) {
    printf("User can read this file\n");
}
if (statbuf.st_mode & S_IWUSR) {
    printf("User can write this file\n");
}
if (statbuf.st_mode & S_IXUSR) {
    printf("User can execute this file\n");
}
```

> You can similarly check group (S_IRGRP, S_IWGRP, S_IXGRP) and others (S_IROTH, etc.).

---

## References

- man 2 stat
- man 2 lstat
- man 7 inode

