#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

// Function declarations
void simple_display(const char *path);
void handle_long_listing(const char *path);
void print_permissions(mode_t mode);

int main(int argc, char *argv[]) {
    int opt;
    bool long_listing = false; // for -l option

    // Parse command-line options
    while ((opt = getopt(argc, argv, "l")) != -1) {
        switch (opt) {
            case 'l':
                long_listing = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [directory]\n", argv[0]);
                return 1;
        }
    }

    // Determine directory to list
    const char *path = (optind < argc) ? argv[optind] : ".";

    if (long_listing)
        handle_long_listing(path);
    else
        simple_display(path);

    return 0;
}

// ✅ Basic version: just print file names
void simple_display(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') // skip hidden files
            printf("%s  ", entry->d_name);
    }
    printf("\n");
    closedir(dir);
}

// ✅ Convert st_mode into rwxrwxrwx string
void print_permissions(mode_t mode) {
    printf( (S_ISDIR(mode)) ? "d" : "-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

// ✅ Long listing: stat(), getpwuid(), getgrgid(), ctime()
void handle_long_listing(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct stat fileStat;
    char fullPath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
            continue; // skip hidden

        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        if (lstat(fullPath, &fileStat) == -1) {
            perror("lstat");
            continue;
        }

        // Permissions
        print_permissions(fileStat.st_mode);
        printf(" ");

        // Link count
        printf("%2ld ", fileStat.st_nlink);

        // Owner name
        struct passwd *pw = getpwuid(fileStat.st_uid);
        struct group *gr = getgrgid(fileStat.st_gid);
        printf("%s %s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        // File size
        printf("%5ld ", fileStat.st_size);

        // Modification time
        char *time_str = ctime(&fileStat.st_mtime);
        time_str[strlen(time_str) - 1] = '\0'; // remove newline
        printf("%s ", time_str);

        // File name
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}
