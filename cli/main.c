#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "commands.h"

void print_help() {
    printf("Usage: [-h | --help] creg <command> [<args>]");
    printf("\nCommands:\n");
    printf("  login     Login using your USF ID");
    printf("  add       Add a course to main or current plan");
    printf("  browse    Search for classes offered this semester");
    printf("  plan      Set working plan");
    printf("  apply     Register for classes in a plan");
    printf("  cbrowse   Browse catalog for all USF courses");
    printf("  degree    Show degree progress");
    printf("  schedule  Show current weekly schedule");
}

void handle_login(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "No argument found for creg login");
        return 1;
    }
    
    login(argv[2]);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    // handle global options before command
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_help();
        return 0;
    }

    // handle commands
    const char* command = argv[1];
    if (strcmp(command, "login") == 0) {
        handle_login(argc, argv);
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        print_help();
        return 1;
    }
}
