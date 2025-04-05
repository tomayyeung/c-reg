#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include "user.h"

char *get_user_file_path() {
    const char *home = getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;
    
    char *path = malloc(strlen(home) + strlen(USER_FILE) + 2);
    sprintf(path, "%s/%s", home, USER_FILE);
    return path;
}

char *load_username() {
    char *path = get_user_file_path();
    FILE *f = fopen(path, "r");
    free(path);

    if (!f) return NULL; // file doesn't exist yet

    char *username = malloc(100);
    if (fgets(username, 100, f) == NULL) {
        fclose(f);
        free(username);
        return NULL;
    }

    username[strcspn(username, "\n")] = 0; // strip trailing newline
    fclose(f);
    return username;
}

void save_username(const char *username) {
    char *path = get_user_file_path();
    printf("path: %s\n", path);
    FILE *f = fopen(path, "w");
    if (!f) {
        perror("Could not save username");
        free(path);
        return;
    }

    fprintf(f, "%s\n", username);
    fclose(f);
    free(path);
}