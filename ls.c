#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_DIR     "\033[1;34m"
#define COLOR_EXEC    "\033[1;32m"
#define COLOR_LINK    "\033[1;36m"
#define COLOR_ARCHIVE "\033[1;31m"

// Function to print file with color
void print_colored(const char *name, struct stat *st) {
    if (S_ISDIR(st->st_mode)) {
        printf(COLOR_DIR "%s" COLOR_RESET "  ", name);
    } else if (S_ISLNK(st->st_mode)) {
        printf(COLOR_LINK "%s" COLOR_RESET "  ", name);
    } else if (st->st_mode & S_IXUSR) {
        printf(COLOR_EXEC "%s" COLOR_RESET "  ", name);
    } else if (strstr(name, ".tar") || strstr(name, ".gz") || strstr(name, ".zip")) {
        printf(COLOR_ARCHIVE "%s" COLOR_RESET "  ", name);
    } else {
        printf("%s  ", name);
    }
}

// Function for long listing
void print_long(struct stat *st, const char *name) {
    char perms[11] = "----------";
    if (S_ISDIR(st->st_mode)) perms[0] = 'd';
    if (S_ISLNK(st->st_mode)) perms[0] = 'l';
    if (st->st_mode & S_IRUSR) perms[1] = 'r';
    if (st->st_mode & S_IWUSR) perms[2] = 'w';
    if (st->st_mode & S_IXUSR) perms[3] = 'x';
    if (st->st_mode & S_IRGRP) perms[4] = 'r';
    if (st->st_mode & S_IWGRP) perms[5] = 'w';
    if (st->st_mode & S_IXGRP) perms[6] = 'x';
    if (st->st_mode & S_IROTH) perms[7] = 'r';
    if (st->st_mode & S_IWOTH) perms[8] = 'w';
    if (st->st_mode & S_IXOTH) perms[9] = 'x';

    struct passwd *pw = getpwuid(st->st_uid);
    struct group  *gr = getgrgid(st->st_gid);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st->st_mtime));

    printf("%s %2ld %s %s %6ld %s %s\n",
           perms,
           st->st_nlink,
           pw ? pw->pw_name : "",
           gr ? gr->gr_name : "",
           st->st_size,
           timebuf,
           name);
}

// Comparator for alphabetical sorting
int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// Core function to list a directory
void do_ls(const char *dirname, int long_flag, int horizontal_flag, int recursive_flag) {
    DIR *dp = opendir(dirname);
    if (!dp) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    char **names = NULL;
    size_t count = 0;

    while ((entry = readdir(dp)) != NULL) {
        names = realloc(names, sizeof(char*) * (count + 1));
        names[count] = strdup(entry->d_name);
        count++;
    }
    closedir(dp);

    qsort(names, count, sizeof(char*), compare);

    if (recursive_flag)
        printf("%s:\n", dirname);

    for (size_t i = 0; i < count; i++) {
        if (long_flag || horizontal_flag) {
            struct stat st;
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, names[i]);
            if (lstat(fullpath, &st) == -1) {
                perror("lstat");
                continue;
            }

            if (long_flag) {
                print_long(&st, names[i]);
            } else if (horizontal_flag) {
                print_colored(names[i], &st);
            }

            // Recursive call if directory
            if (recursive_flag && S_ISDIR(st.st_mode) && strcmp(names[i], ".") != 0 && strcmp(names[i], "..") != 0) {
                printf("\n");
                do_ls(fullpath, long_flag, horizontal_flag, recursive_flag);
            }
        } else {
            struct stat st;
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, names[i]);
            if (lstat(fullpath, &st) == -1) continue;
            print_colored(names[i], &st);
        }
    }

    if (!long_flag && horizontal_flag) printf("\n");

    for (size_t i = 0; i < count; i++) free(names[i]);
    free(names);
}

// Main function
int main(int argc, char *argv[]) {
    int long_flag = 0;
    int horizontal_flag = 0;
    int recursive_flag = 0;

    int opt;
    while ((opt = getopt(argc, argv, "lxR")) != -1) {
        switch (opt) {
            case 'l': long_flag = 1; break;
            case 'x': horizontal_flag = 1; break;
            case 'R': recursive_flag = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-x] [-R] [file...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind == argc) {
        do_ls(".", long_flag, horizontal_flag, recursive_flag);
    } else {
        for (int i = optind; i < argc; i++) {
            struct stat st;
            if (lstat(argv[i], &st) == -1) {
                fprintf(stderr, "%s: No such file or directory\n", argv[i]);
                continue;
            }
            if (S_ISDIR(st.st_mode)) {
                do_ls(argv[i], long_flag, horizontal_flag, recursive_flag);
            } else {
                if (long_flag) print_long(&st, argv[i]);
                else print_colored(argv[i], &st);
                if (horizontal_flag) printf("\n");
            }
        }
    }

    return 0;
}
