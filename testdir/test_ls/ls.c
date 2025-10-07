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

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_PINK    "\033[0;35m"
#define COLOR_REVERSE "\033[7m"

// Struct to hold file info for sorting
typedef struct {
    char name[256];
    mode_t mode;
} FileEntry;

// Function to print file with color based on type
void print_colored(const char *name, mode_t mode) {
    if (S_ISDIR(mode))
        printf(COLOR_BLUE "%s" COLOR_RESET "  ", name);
    else if (S_ISLNK(mode))
        printf(COLOR_PINK "%s" COLOR_RESET "  ", name);
    else if (S_ISREG(mode) && (mode & S_IXUSR))
        printf(COLOR_GREEN "%s" COLOR_RESET "  ", name);
    else if (strstr(name, ".tar") || strstr(name, ".gz") || strstr(name, ".zip"))
        printf(COLOR_RED "%s" COLOR_RESET "  ", name);
    else if (S_ISCHR(mode) || S_ISBLK(mode) || S_ISFIFO(mode) || S_ISSOCK(mode))
        printf(COLOR_REVERSE "%s" COLOR_RESET "  ", name);
    else
        printf("%s  ", name);
}

// Function to print long listing
void print_long(const char *name, const char *path, mode_t mode, ino_t ino) {
    struct stat st;
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);
    if (lstat(fullpath, &st) == -1) {
        perror("lstat");
        return;
    }

    char perms[11] = "----------";
    if (S_ISDIR(st.st_mode)) perms[0] = 'd';
    if (S_ISLNK(st.st_mode)) perms[0] = 'l';
    if (S_ISCHR(st.st_mode)) perms[0] = 'c';
    if (S_ISBLK(st.st_mode)) perms[0] = 'b';
    if (S_ISFIFO(st.st_mode)) perms[0] = 'p';
    if (S_ISSOCK(st.st_mode)) perms[0] = 's';

    if (st.st_mode & S_IRUSR) perms[1] = 'r';
    if (st.st_mode & S_IWUSR) perms[2] = 'w';
    if (st.st_mode & S_IXUSR) perms[3] = 'x';
    if (st.st_mode & S_IRGRP) perms[4] = 'r';
    if (st.st_mode & S_IWGRP) perms[5] = 'w';
    if (st.st_mode & S_IXGRP) perms[6] = 'x';
    if (st.st_mode & S_IROTH) perms[7] = 'r';
    if (st.st_mode & S_IWOTH) perms[8] = 'w';
    if (st.st_mode & S_IXOTH) perms[9] = 'x';

    struct passwd *pw = getpwuid(st.st_uid);
    struct group  *gr = getgrgid(st.st_gid);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));

    printf("%s %2lu %s %s %6ld %s %s\n",
           perms,
           st.st_nlink,
           pw ? pw->pw_name : "unknown",
           gr ? gr->gr_name : "unknown",
           st.st_size,
           timebuf,
           name);
}

// Compare function for alphabetical sort
int compare_files(const void *a, const void *b) {
    FileEntry *fa = (FileEntry *)a;
    FileEntry *fb = (FileEntry *)b;
    return strcasecmp(fa->name, fb->name);
}

// Function to list directory
void list_dir(const char *path, int long_flag, int horizontal_flag, int recursive_flag) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    FileEntry files[1024];
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        files[count].mode = 0;
        strcpy(files[count].name, entry->d_name);

        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (lstat(fullpath, &st) != -1)
            files[count].mode = st.st_mode;

        count++;
    }

    qsort(files, count, sizeof(FileEntry), compare_files);

    for (int i = 0; i < count; i++) {
        if (long_flag)
            print_long(files[i].name, path, files[i].mode, 0);
        else
            print_colored(files[i].name, files[i].mode);
    }

    if (!long_flag) printf("\n");
    closedir(dir);

    if (recursive_flag) {
        for (int i = 0; i < count; i++) {
            char fullpath[512];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, files[i].name);
            if (S_ISDIR(files[i].mode)) {
                printf("\n%s:\n", fullpath);
                list_dir(fullpath, long_flag, horizontal_flag, recursive_flag);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int long_flag = 0, horizontal_flag = 0, recursive_flag = 0;
    int opt;

    while ((opt = getopt(argc, argv, "lxR")) != -1) {
        switch (opt) {
            case 'l': long_flag = 1; break;
            case 'x': horizontal_flag = 1; break;
            case 'R': recursive_flag = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-x] [-R] [directory]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    char *dir_path;
    if (optind < argc)
        dir_path = argv[optind];
    else
        dir_path = ".";

    list_dir(dir_path, long_flag, horizontal_flag, recursive_flag);

    return 0;
}
