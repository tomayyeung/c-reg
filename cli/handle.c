#include <getopt.h>

#include "commands.h"
#include "commands_helper.h"
#include "handle.h"

int handle_art() {
    return art();
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
