#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "commands.h"

void print_help() {
    printf("Usage: [-h | --help] creg <command> [<args>]");
    printf("\nCommands:\n");
    printf("  login     Login using your USF ID\n");
    printf("  add       Add a course to main or current plan\n");
    printf("  browse    Search for classes offered this semester\n");
    printf("  plan      Set working plan\n");
    printf("  apply     Register for classes in a plan\n");
    printf("  cbrowse   Browse catalog for all USF courses\n");
    printf("  degree    Show degree progress\n");
    printf("  schedule  Show current weekly schedule\n");
}

int handle_login(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "No argument found for creg login\n");
        return 1;
    }
    
    return login(argv[2]);
}

int handle_add(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "No argument found for creg add\n");
        return 1;
    }

    int crn = atoi(argv[2]);
    if (crn == 0) {
        fprintf(stderr, "Could not read CRN for creg add\n");
        return 1;
    }
    
    if (argc == 3) { // main plan
        add(crn, "");
    } else {
        add(crn, argv[3]);
    }
    return 0;
}

int handle_browse(int argc, char** argv) {
    char* subject = "";

    static struct option long_options[] = {
        {"subject", required_argument, 0, 's'},
        {0, 0, 0, 0},
    };
    const char* short_options = "s:";
    
    int opt;
    optind = 2; // skip program and command
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 's':
                subject = optarg;
                break;
            
            default:
                fprintf(stderr, "Invalid usage of creg browse\n");
                return 1;
        }
    }

    return browse(subject, 0, 0, "", "", "", "");
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
    } else if (strcmp(command, "add") == 0) {
        handle_add(argc, argv);
    } else if (strcmp(command, "browse") == 0) {
        handle_browse(argc, argv);
    }
    else {
        fprintf(stderr, "Unknown command: %s\n", command);
        print_help();
        return 1;
    }
}
