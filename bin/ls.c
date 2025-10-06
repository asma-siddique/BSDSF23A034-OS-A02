#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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

    printf("Number of files: %zu\n", count);
    printf("Longest filename length: %zu\n", max_len);

    for (size_t i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);

    return 0;
}
