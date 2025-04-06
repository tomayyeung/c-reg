#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "commands.h"
#include "handle.h"
#include "help.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    int help = 0;
    int command_i = 1;

    // handle global options before command
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        command_i++;
        help = 1;
    }

    mongoc_init();
    // Create a Mongo client object and connect to MongoDB server
    mongoc_client_t *client = mongoc_client_new("mongodb+srv://leozhang2024:bzZhMIdXY8hDYXxe@course-reg-cluster.pyj2eqo.mongodb.net/?retryWrites=true&w=majority&appName=course-reg-cluster");

    if (!client) {
        fprintf(stderr, "Failed to create client\n");
        return 1;
    }

    // handle commands
    const char* command = argv[command_i];
    if (strcmp(command, "art") == 0) {
        if (help) {
            print_help_art();
            return 0;
        }
        return handle_art();
    } else if (strcmp(command, "login") == 0) {
        if (help) {
            print_help_login();
            return 0;
        }
        return handle_login(argc, argv, client);
    } else if (strcmp(command, "logout") == 0) {
        if (help) {
            print_help_logout();
            return 0;
        }
        return handle_logout();
    } else if (strcmp(command, "add") == 0) {
        if (help) {
            print_help_add();
            return 0;
        }
        return handle_add(argc, argv, client);
    } else if (strcmp(command, "rm") == 0) {
        if (help) {
            print_help_rm();
            return 0;
        }
        return handle_rm(argc, argv, client);
    } else if (strcmp(command, "rmplan") == 0) {
        if (help) {
            print_help_rmplan();
            return 0;
        }
        return handle_rmplan(argc, argv, client);
    } else if (strcmp(command, "view") == 0) {
        if (help) {
            print_help_view();
            return 0;
        }
        return handle_view(argc, argv, client);
    } else if (strcmp(command, "viewplans") == 0) {
        if (help) {
            print_help_viewplans();
            return 0;
        }
        return handle_viewplans(client);
    } else if (strcmp(command, "browse") == 0) {
        if (help) {
            print_help_browse();
            return 0;
        }
        return handle_browse(argc, argv, client);
    } else if (strcmp(command, "apply") == 0) {
        if (help) {
            print_help_apply();
            return 0;
        }
        return handle_apply(argc, argv, client);
    } else if (strcmp(command, "schedule") == 0) {
        if (help) {
            print_help_schedule();
            return 0;
        }
        return handle_schedule(argc, argv, client);
    }
    else {
        print_help();
        if (help) return 0;
        fprintf(stderr, "Unknown command: %s\n", command);
        return 1;
    }
}
