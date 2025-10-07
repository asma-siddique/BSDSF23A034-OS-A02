#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

// ANSI color codes
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define MAGENTA "\033[0;35m"
#define REVERSE "\033[7m"
#define RESET "\033[0m"

// Function to compare filenames for qsort (alphabetical)
int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// Function to print file with color based on type
void print_colored(const char *name, const struct stat *st) {
    if (S_ISDIR(st->st_mode)) {
        printf(BLUE "%s" RESET, name);
    } else if (S_ISLNK(st->st_mode)) {
        printf(MAGENTA "%s" RESET, name);
    } else if (st->st_mode & S_IXUSR) {
        printf(GREEN "%s" RESET, name);
    } else if (strstr(name, ".tar") || strstr(name, ".gz") || strstr(name, ".zip")) {
        printf(RED "%s" RESET, name);
    } else if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode) || S_ISFIFO(st->st_mode) || S_ISSOCK(st->st_mode)) {
        printf(REVERSE "%s" RESET, name);
    } else {
        printf("%s", name);
    }
}

// Print long listing like ls -l
void print_long(const char *name, const struct stat *st) {
    char perms[11];
    perms[0] = S_ISDIR(st->st_mode) ? 'd' :
               S_ISLNK(st->st_mode) ? 'l' :
               S_ISCHR(st->st_mode) ? 'c' :
               S_ISBLK(st->st_mode) ? 'b' :
               S_ISFIFO(st->st_mode) ? 'p' :
               S_ISSOCK(st->st_mode) ? 's' : '-';
    perms[1] = (st->st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (st->st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (st->st_mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (st->st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (st->st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (st->st_mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (st->st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (st->st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (st->st_mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';

    struct passwd *pw = getpwuid(st->st_uid);
    struct group  *gr = getgrgid(st->st_gid);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st->st_mtime));

    printf("%s %2ld %s %s %6ld %s ", perms, st->st_nlink,
           pw ? pw->pw_name : "?", gr ? gr->gr_name : "?",
           st->st_size, timebuf);
    print_colored(name, st);
    printf("\n");
}

int main(int argc, char *argv[]) {
    int long_listing = 0;
    int horizontal = 0;
    const char *dirpath = ".";

    // Parse command line args
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) long_listing = 1;
        else if (strcmp(argv[i], "-x") == 0) horizontal = 1;
        else dirpath = argv[i];
    }

    DIR *dir = opendir(dirpath);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    char **files = NULL;
    size_t count = 0;

    while ((entry = readdir(dir)) != NULL) {
        files = realloc(files, sizeof(char*) * (count + 1));
        files[count] = strdup(entry->d_name);
        count++;
    }
    closedir(dir);

    // Alphabetical sort
    qsort(files, count, sizeof(char*), compare);

    for (size_t i = 0; i < count; i++) {
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, files[i]);
        struct stat st;
        if (lstat(filepath, &st) < 0) {
            perror("lstat");
            continue;
        }

        if (long_listing) {
            print_long(files[i], &st);
        } else {
            print_colored(files[i], &st);
            if (horizontal) printf("  ");
            else printf("\n");
        }
    }
    if (horizontal) printf("\n");

    for (size_t i = 0; i < count; i++) free(files[i]);
    free(files);
    return 0;
}
