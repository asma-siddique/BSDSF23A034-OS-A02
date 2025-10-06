#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main() {
    DIR *dir;
    struct dirent *entry;
    char **filenames = NULL;
    size_t count = 0;
    size_t max_len = 0;

    dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    // Gather all filenames
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char **tmp = realloc(filenames, (count + 1) * sizeof(char *));
        if (!tmp) {
            perror("realloc");
            for (size_t i = 0; i < count; i++) free(filenames[i]);
            free(filenames);
            closedir(dir);
            return 1;
        }
        filenames = tmp;

        filenames[count] = strdup(entry->d_name);
        if (!filenames[count]) {
            perror("strdup");
            for (size_t i = 0; i < count; i++) free(filenames[i]);
            free(filenames);
            closedir(dir);
            return 1;
        }

        size_t len = strlen(entry->d_name);
        if (len > max_len)
            max_len = len;

        count++;
    }
    closedir(dir);

    // Get terminal width
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int term_width = w.ws_col;

    // Calculate number of columns and rows
    int col_width = max_len + 2; // 2 spaces padding
    int cols = term_width / col_width;
    if (cols == 0) cols = 1;
    int rows = (count + cols - 1) / cols; // ceil division

    // Print filenames down then across
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int idx = c * rows + r;
            if (idx < count) {
                printf("%-*s", col_width, filenames[idx]);
            }
        }
        printf("\n");
    }

    // Free memory
    for (size_t i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);

    return 0;
}
