#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "commands.h"
#include "commands_helper.h"

void print_help() {
    printf("Usage: creg [-h | --help] <command> [<args>]");
    printf("\nCommands:\n");
    printf("  login     Login using your USF ID\n");
    printf("  logout    Log out from creg\n");
    printf("  add       Add a section to main or plan\n");
    printf("  rm        Remove a course from main or plan\n");
    printf("  rmplan    Remove a plan and all sections added\n");
    printf("  view      View sections added to main or plan\n");
    printf("  viewplans View a list of all plans\n");
    printf("  browse    Search for classes offered this semester\n");
    printf("  apply     Add all for sections in a plan to main registration\n");
    // printf("  cbrowse   Browse catalog for all USF courses\n");
    // printf("  degree    Show degree progress\n");
    printf("  schedule  Show weekly schedule of main or plan\n");
}

void print_help_login() {
    printf("Usage: creg login [username]\n");
    printf("  You will then be prompted for your password.\n");
}

void print_help_logout() {
    printf("Usage: creg logout\n");
}

void print_help_add() {
    printf("Usage: creg add [crn] [-p | --plan=<name>]\n");
    printf("  Adds given crn to specified plan\n");
    printf("  If no plan is specified, adds to main registration\n");
    printf("  If plan does not exist, creates a new plan with the given name\n");
}

void print_help_rm() {
    printf("Usage: creg rm [crn] [-p | --plan=<name>]\n");
    printf("  Removes given crn from specified plan\n");
    printf("  If no plan is specified, removes from main registration\n");
}

void print_help_rmplan() {
    printf("Usage: creg rmplan [name]\n");
    printf("  Removes given plan and all added sections\n");
    printf("  This command is irreversible\n");
}

void print_help_view() {
    printf("Usage: creg view [-p | --plan=<name>]\n");
    printf("  Views all sections aded to specified plan\n");
    printf("  If no plan is specified, views main registration\n");
}

void print_help_viewplans() {
    printf("Usage: creg viewplans\n");
    printf("  Views all created plans\n");
}

void print_help_browse() {
    printf("Usage: creg browse [-s | --subject<subject>] [-n | --number<number>] [-I | --instruction<instrmethod>] \n");
    printf("    [-a | --aatributes<attributes>]  [-i | --instructor<name>] [-k | --keywords<keywords>]\n");
    printf("  Browses all sections with given search queries:\n");
    printf("    [-s | --subject<subject>] searches for the given subject, eg CS.\n");
    printf("    [-n | --number<number>] searches for a specified course number, eg 221 for CS 221.\n");
    printf("    [-I | --instruction<instrmethod>] searches for the given instructional method: InPerson, Online, Hybrid.\n");
    printf("    [-a | --attributes<attributes>] to be added\n");
    printf("    [-i | --instructor<name>] searches for the given instructor name. Place the name inside quotes, eg \"Paul Haskell\".\n");
    printf("    [-k | --keywords<keywords>] to be added\n");
}

void print_help_apply() {
    printf("Usage: creg apply [name]\n");
    printf("  Empties main registration and adds all sections in given plan.\n");
}

void print_help_schedule() {
    printf("Usage: creg schedule [-p | --plan=<name>]\n");
    printf("  Displays weekly schedule of given plan.\n");
    printf("  Sections are sorted by day and time.\n");
}

int handle_login(int argc, char** argv, mongoc_client_t* client) {
    if (argc < 3) {
        fprintf(stderr, "No argument found for creg login\n");
        return 1;
    }

    mongoc_collection_t* users_collection = mongoc_client_get_collection(client, "c-reg_DB", "users");
    
    return login(argv[2], users_collection);
}

int handle_logout() {
    return logout();
}

int handle_add(int argc, char** argv, mongoc_client_t* client) {
    if (argc < 3) {
        fprintf(stderr, "No argument found for creg add\n");
        return 1;
    }

    int crn = atoi(argv[2]);
    if (crn == 0) {
        fprintf(stderr, "Could not read CRN for creg add\n");
        return 1;
    }

    char plan[256];
    *plan = 0;

    static struct option long_options[] = {
        {"plan", required_argument, 0, 'p'},
    };
    const char* short_options = "p:";

    int opt;
    optind = 2; // skip program and command
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                strncpy(plan,  optarg, 256);
                break;
        }
    }

    mongoc_collection_t* plans_collection = mongoc_client_get_collection(client, "c-reg_DB", "plans");
    mongoc_collection_t* sections_collection = mongoc_client_get_collection(client, "c-reg_DB", "sections");

    return add(crn, plan, plans_collection, sections_collection);
}

int handle_rm(int argc, char** argv, mongoc_client_t* client) {
    if (argc < 3) {
        fprintf(stderr, "No argument found for creg rm\n");
        return 1;
    }

    int crn = atoi(argv[2]);
    if (crn == 0) {
        fprintf(stderr, "Could not read CRN for creg rm\n");
        return 1;
    }

    char plan[256];
    *plan = 0;

    static struct option long_options[] = {
        {"plan", required_argument, 0, 'p'},
    };
    const char* short_options = "p:";

    int opt;
    optind = 2; // skip program and command
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                strncpy(plan,  optarg, 256);
                break;
        }
    }

    mongoc_collection_t* plans_collection = mongoc_client_get_collection(client, "c-reg_DB", "plans");
    mongoc_collection_t* sections_collection = mongoc_client_get_collection(client, "c-reg_DB", "sections");

    return rm(crn, plan, plans_collection, sections_collection);    
}

int handle_rmplan(int argc, char** argv, mongoc_client_t* client) {
    if (argc < 3) {
        fprintf(stderr, "No argument found for creg rmplan\n");
        return 1;
    }
    if (strcmp(argv[2], "main") == 0) {
        fprintf(stderr, "Cannot remove main registration\n");
        return 1;
    }

    mongoc_collection_t* plans_collection = mongoc_client_get_collection(client, "c-reg_DB", "plans");
    return rmplan(argv[2], plans_collection);
}

int handle_view(int argc, char** argv, mongoc_client_t* client) {
    char plan[256];
    *plan = 0;

    static struct option long_options[] = {
        {"plan", required_argument, 0, 'p'},
    };
    const char* short_options = "p:";

    int opt;
    optind = 2; // skip program and command
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                strncpy(plan,  optarg, 256);
                break;
        }
    }

    mongoc_collection_t* plans_collection = mongoc_client_get_collection(client, "c-reg_DB", "plans");
    return view(plan, plans_collection);    
}

int handle_viewplans(mongoc_client_t* client) {
    mongoc_collection_t* plans_collection = mongoc_client_get_collection(client, "c-reg_DB", "plans");
    return viewplans(plans_collection);
}

int handle_browse(int argc, char** argv, mongoc_client_t* client) {
    mongoc_collection_t* sections_collection = mongoc_client_get_collection(client, "c-reg_DB", "sections");
    mongoc_collection_t* courses_collection = mongoc_client_get_collection(client, "c-reg_DB", "courses");

    // options
    char subject[5] = "";
    char number[16] = "";
    enum InstructionMode instruction = 3;
    int n_attrs = 0;
    enum Attribute attrs[16];
    char instructor[256] = "";
    int n_keywords = 0;
    char** keywords;

    // set up options parsing
    static struct option long_options[] = {
        {"subject", required_argument, 0, 's'},
        {"number", required_argument, 0, 'n'},
        {"instruction", required_argument, 0, 'I'},
        {"attributes", required_argument, 0, 'a'},
        {"instructor", required_argument, 0, 'i'},
        {"keywords", required_argument, 0, 'k'},
        {0, 0, 0, 0},
    };
    const char* short_options = "s:n:I:a:i:k:";

    char* delimiter = " ";
    char* token;
    char* str_copy;
    
    int opt;
    optind = 2; // skip program and command
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 's':
                strcpy(subject, optarg);
                break;

            case 'n':
                // if ((number = atoi(optarg)) == 0) {
                //     fprintf(stderr, "Invalid argument for creg browse --number\n");
                // }
                strcpy(number, optarg);
                break;

            case 'I':
                if ((instruction = str_to_instr_mode(optarg)) == -1) {
                    fprintf(stderr, "Invalid argument for creg browse --instruction\n");
                    return 1;
                }
                // printf("instruction %d\n", instruction);
                break;

            case 'a': // segfaults on incorrect input
                // read all attribute inputs
                str_copy = strdup(optarg); // Create a copy of the string

                if (str_copy == NULL) {
                    fprintf(stderr, "strdup failed for creg browse --attributes\n");
                    return 1;
                }
                
                token = strtok(str_copy, delimiter);

                while (token != NULL) {
                    if (n_attrs >= 16) {
                        fprintf(stderr, "Too many arguments for creg browse --attributes\n");
                        return 1;
                    }
                    if ((attrs[n_attrs++] = str_to_attr(token)) == 0) {
                        fprintf(stderr, "Invalid argument for creg browse --attributes\n");
                        return 1;
                    }
                    token = strtok(NULL, delimiter); // Subsequent calls use NULL
                }

                free(str_copy);
                break;

            case 'i':
                if (strlen(optarg) > 256) {
                    fprintf(stderr, "Too long of an argument for creg browse --instructor\n");
                    return 1;
                }
                strcpy(instructor, optarg);
                break;

            case 'k':
                // read all keyword inputs
                str_copy = strdup(optarg); // Create a copy of the string

                if (str_copy == NULL) {
                    fprintf(stderr, "strdup failed for creg browse --keywords\n");
                    return 1;
                }
                
                token = strtok(str_copy, delimiter);

                while (token != NULL) {
                    if (n_keywords >= 16) {
                        fprintf(stderr, "Too many arguments for creg browse --keywords\n");
                        return 1;
                    }
                    if ((keywords[n_keywords++] = token) == 0) {
                        fprintf(stderr, "Invalid argument for creg browse --keywords\n");
                        return 1;
                    }
                    token = strtok(NULL, delimiter); // Subsequent calls use NULL
                }

                free(str_copy);
                free(token);
                break;

            default:
                fprintf(stderr, "Invalid usage of creg browse\n");
                return 1;
        }
    }

    return browse(subject, number, instruction, n_attrs, attrs, instructor, n_keywords, keywords, sections_collection, courses_collection);
}

int handle_apply(int argc, char** argv, mongoc_client_t* client) {
    if (argc < 3) {
        fprintf(stderr, "No argument found for creg apply\n");
        return 1;
    }

    mongoc_collection_t* plans_collection = mongoc_client_get_collection(client, "c-reg_DB", "plans");
    return apply(argv[2], plans_collection);
}

int handle_schedule(int argc, char** argv, mongoc_client_t* client) {
    char plan[256];
    *plan = 0;

    static struct option long_options[] = {
        {"plan", required_argument, 0, 'p'},
    };
    const char* short_options = "p:";

    int opt;
    optind = 2; // skip program and command
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                strncpy(plan,  optarg, 256);
                break;
        }
    }

    mongoc_collection_t* courses_collection = mongoc_client_get_collection(client, "c-reg_DB", "courses");
    mongoc_collection_t* plans_collection = mongoc_client_get_collection(client, "c-reg_DB", "plans");
    mongoc_collection_t* sections_collection = mongoc_client_get_collection(client, "c-reg_DB", "sections");
    return schedule(plan, courses_collection, plans_collection, sections_collection);
}

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
    if (strcmp(command, "login") == 0) {
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
