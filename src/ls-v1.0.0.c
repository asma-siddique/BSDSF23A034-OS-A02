#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

#define MAX_NAME_LEN 256

// -------------------------
// Feature 5: Alphabetical sort comparison
int compare_filenames(const void *a, const void *b) {
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    return strcmp(str1, str2);
}

// -------------------------
// Feature 1 & 3: Read directory and store filenames dynamically
char **read_directory(const char *path, int *count) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    int capacity = 10;
    int n = 0;
    char **files = malloc(sizeof(char *) * capacity);
    if (!files) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (n >= capacity) {
            capacity *= 2;
            files = realloc(files, sizeof(char *) * capacity);
            if (!files) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
        files[n++] = strdup(entry->d_name);
    }
    closedir(dir);

    // -------------------------
    // Feature 5: Sort alphabetically
    qsort(files, n, sizeof(char *), compare_filenames);

    *count = n;
    return files;
}

// -------------------------
// Feature 1: Default column display (down then across)
void print_default(char **files, int n) {
    int columns = 3; // adjust as needed
    int rows = (n + columns - 1) / columns;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            int index = c * rows + r;
            if (index < n)
                printf("%-25s", files[index]);
        }
        printf("\n");
    }
}

// -------------------------
// Feature 2: Horizontal display (-x)
void print_horizontal(char **files, int n) {
    int columns = 3; // adjust as needed
    for (int i = 0; i < n; i++) {
        printf("%-25s", files[i]);
        if ((i + 1) % columns == 0)
            printf("\n");
    }
    if (n % columns != 0) printf("\n");
}

// -------------------------
// Feature 1: Long listing (-l)
void print_long_format(char **files, int n) {
    struct stat st;
    char buf[1024];

    for (int i = 0; i < n; i++) {
        if (stat(files[i], &st) == -1) {
            perror("stat");
            continue;
        }

        printf( (S_ISDIR(st.st_mode)) ? "d" : "-");
        printf( (st.st_mode & S_IRUSR) ? "r" : "-");
        printf( (st.st_mode & S_IWUSR) ? "w" : "-");
        printf( (st.st_mode & S_IXUSR) ? "x" : "-");
        printf( (st.st_mode & S_IRGRP) ? "r" : "-");
        printf( (st.st_mode & S_IWGRP) ? "w" : "-");
        printf( (st.st_mode & S_IXGRP) ? "x" : "-");
        printf( (st.st_mode & S_IROTH) ? "r" : "-");
        printf( (st.st_mode & S_IWOTH) ? "w" : "-");
        printf( (st.st_mode & S_IXOTH) ? "x" : "-");

        printf(" %2ld", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group  *gr = getgrgid(st.st_gid);
        printf(" %-8s %-8s", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        printf(" %6lld", (long long)st.st_size);

        struct tm *tm_info = localtime(&st.st_mtime);
        strftime(buf, sizeof(buf), "%b %d %H:%M", tm_info);
        printf(" %s %s\n", buf, files[i]);
    }
}

// -------------------------
// Main function
int main(int argc, char *argv[]) {
    int flag_l = 0, flag_x = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            flag_l = 1;
        else if (strcmp(argv[i], "-x") == 0)
            flag_x = 1;
    }

    int n;
    char **files = read_directory(".", &n);

    if (flag_l)
        print_long_format(files, n);
    else if (flag_x)
        print_horizontal(files, n);
    else
        print_default(files, n);

    // Free memory
    for (int i = 0; i < n; i++)
        free(files[i]);
    free(files);

    return 0;
}
