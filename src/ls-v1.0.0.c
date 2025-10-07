#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_PINK    "\033[0;35m"
#define COLOR_REVERSE "\033[7m"

// Helper function: print file with color based on type
void print_colored(const char *name, const struct stat *st) {
    if (S_ISDIR(st->st_mode)) {
        printf("%s%s%s  ", COLOR_BLUE, name, COLOR_RESET);
    } else if (S_ISLNK(st->st_mode)) {
        printf("%s%s%s  ", COLOR_PINK, name, COLOR_RESET);
    } else if (st->st_mode & S_IXUSR || st->st_mode & S_IXGRP || st->st_mode & S_IXOTH) {
        printf("%s%s%s  ", COLOR_GREEN, name, COLOR_RESET);
    } else if (strstr(name, ".tar") || strstr(name, ".gz") || strstr(name, ".zip")) {
        printf("%s%s%s  ", COLOR_RED, name, COLOR_RESET);
    } else if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode) || S_ISSOCK(st->st_mode) || S_ISFIFO(st->st_mode)) {
        printf("%s%s%s  ", COLOR_REVERSE, name, COLOR_RESET);
    } else {
        printf("%s  ", name);
    }
}

// Comparison function for qsort
int cmpstr(const void *a, const void *b) {
    const char *pa = *(const char **)a;
    const char *pb = *(const char **)b;
    return strcmp(pa, pb);
}

// Display horizontal (default or -x)
void display_horizontal(char **names, int count) {
    for (int i = 0; i < count; i++) {
        struct stat st;
        if (lstat(names[i], &st) == -1) continue;
        print_colored(names[i], &st);
    }
    printf("\n");
}

// Display long listing (-l)
void display_long(char **names, int count) {
    for (int i = 0; i < count; i++) {
        struct stat st;
        if (lstat(names[i], &st) == -1) continue;
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
        printf(" %5ld ", st.st_nlink);
        printf("%s  ", names[i]);
        print_colored(names[i], &st);
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    char *path = ".";
    int long_listing = 0;
    int horizontal = 0;

    // Parse options
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strchr(argv[i], 'l')) long_listing = 1;
            if (strchr(argv[i], 'x')) horizontal = 1;
        } else {
            path = argv[i];
        }
    }

    // Open directory
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    // Read filenames into array
    struct dirent *entry;
    char **names = NULL;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files (start with '.') if you want
        // if (entry->d_name[0] == '.') continue;
        names = realloc(names, sizeof(char*) * (count + 1));
        names[count] = strdup(entry->d_name);
        count++;
    }
    closedir(dir);

    // Sort filenames
    qsort(names, count, sizeof(char*), cmpstr);

    // Display
    if (long_listing) {
        display_long(names, count);
    } else {
        display_horizontal(names, count);
    }

    // Free memory
    for (int i = 0; i < count; i++) free(names[i]);
    free(names);

    return 0;
}
