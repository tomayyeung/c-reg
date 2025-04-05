#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <bson.h>
#include <mongoc.h>

#include "commands.h"
#include "user.h"

// makes a string lowercase in place
void to_lower(char* s) {
    while (*s != 0) {
        if (*s >= 'A' && *s <= 'Z') *s += 'a' - 'A';
        s++;
    }
}

void get_password(char* password, int max_len) {
    struct termios oldt, newt;
    printf("Enter password: ");
    fflush(stdout);

    // Turn echoing off
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Get password
    fgets(password, max_len, stdin);

    // Turn echoing back on
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\n");

    // Remove newline
    password[strcspn(password, "\n")] = 0;
}

// TO COMPLETE
char* attr_to_str(enum Attribute a) {
    switch (a) {
        case AI: return "ArrupeImmersion";
        case CLN: return "ClinicalSitesNursing";
        case A1: return "CoreA1";
        case A2: return "CoreA2";
        case B1: return "CoreB1";
        case B2: return "CoreB2";
        case C1: return "CoreC1";
        case C2: return "CoreC2";
        case CD: return "CoreCD";
        case CEL: return "CoreCEL";
        case D1: return "CoreD1";
        case D2: return "CoreD2";
        case D3: return "CoreD3";
        case E: return "CoreE";
        case F: return "CoreF";
        default: return 0;
    }
}

// TO COMPLETE
enum Attribute str_to_attr(char* s) {
    to_lower(s);
    if (strcmp(s, "ai") == 0 || strcmp(s, "arrupeimmersion") == 0) return AI;
    else if (strcmp(s, "cln") == 0 || strcmp(s, "clinicalsitesnursing") == 0) return CLN;
    else if (strcmp(s, "a1") == 0 || strcmp(s, "corea1") == 0) return A1;
    else if (strcmp(s, "a2") == 0 || strcmp(s, "corea2") == 0) return A2;
    else if (strcmp(s, "b1") == 0 || strcmp(s, "coreb1") == 0) return B1;
    else if (strcmp(s, "b2") == 0 || strcmp(s, "coreb2") == 0) return B2;
    else if (strcmp(s, "c1") == 0 || strcmp(s, "corec1") == 0) return C1;
    else if (strcmp(s, "c2") == 0 || strcmp(s, "corec2") == 0) return C2;
    else if (strcmp(s, "cd") == 0 || strcmp(s, "corecd") == 0) return CD;
    else if (strcmp(s, "cel") == 0 || strcmp(s, "corecel") == 0) return CEL;
    else if (strcmp(s, "d1") == 0 || strcmp(s, "cored1") == 0) return D1;
    else if (strcmp(s, "d2") == 0 || strcmp(s, "cored2") == 0) return D2;
    else if (strcmp(s, "d3") == 0 || strcmp(s, "cored3") == 0) return D3;
    else if (strcmp(s, "e") == 0 || strcmp(s, "coree") == 0) return E;
    else if (strcmp(s, "f") == 0 || strcmp(s, "coref") == 0) return F;
    return 0;
}

char* instr_mode_to_str(enum InstructionMode i) {
    switch (i) {
        case Hybrid: return "Hybrid";
        case InPerson: return "InPerson";
        case NonTraditional: return "NonTraditional";
        case OnlineAsynch: return "OnlineAsynch";
        case OnlineSynch: return "OnlineSynch";
        case Traditional: return "Traditional";
        default: return 0;
    }
}

enum InstructionMode str_to_instr_mode(char* s) {
    to_lower(s);
    if (strcmp(s, "hybrid") == 0) return Hybrid;
    else if (strcmp(s, "inperson") == 0) return InPerson;
    else if (strcmp(s, "nontraditional") == 0) return NonTraditional;
    else if (strcmp(s, "onlineasynch") == 0) return OnlineAsynch;
    else if (strcmp(s, "OnlineSynch") == 0) return OnlineSynch;
    else if (strcmp(s, "traditional") == 0) return Traditional;
    return 0;
}

int login(char* user, mongoc_collection_t* collection) {
    // Create a filter BSON document (empty filter means all documents)
    bson_t *filter = bson_new();  // Empty filter to fetch all documents
    const bson_t *reply;
    bson_error_t error;

    // Perform the find operation
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);

    // Iterate over the cursor and print each document
    while (mongoc_cursor_next(cursor, &reply)) {
        bson_iter_t iter;

        // get password
        char* password;
        if (bson_iter_init_find(&iter, reply, "password") && BSON_ITER_HOLDS_UTF8(&iter)) {
            password = bson_iter_utf8(&iter, NULL);
        }

        // get username
        if (bson_iter_init_find(&iter, reply, "name") && BSON_ITER_HOLDS_UTF8(&iter)) {
            const char *username = bson_iter_utf8(&iter, NULL);
            if (strcmp(user, username) != 0) {
                continue;
            }

            // get password input
            char password_in[128];
            get_password(password_in, sizeof(password_in));
            if (strcmp(password, password_in) != 0) {
                fprintf(stderr, "Wrong password\n");
                return 1;
            }

            printf("saving username\n");
            
            save_username(user);
        }
    }

    if (mongoc_cursor_error(cursor, &error)) {
        fprintf(stderr, "Cursor error: %s\n", error.message);
    }

    // Cleanup
    bson_destroy(filter);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    mongoc_cleanup();

    printf("login in commands.c\n");
    return 0;
}

int logout() {
    return remove(get_user_file_path());
}

int add(int crn, char* plan) {
    printf("added crn: %d to plan: %s\n", crn, plan);
    return 0;
}

int browse(char subject[5], int number, enum InstructionMode instruction_mode, int n_attrs, enum Attribute attrs[16], char instructor[256], int n_keywords, char** keywords) {
    printf("searching subject: %s\n", subject);
    printf("searching number: %d\n", number);
    printf("searching instruction mode: %s\n", instr_mode_to_str(instruction_mode));
    printf("searching n attrs: %d\n", n_attrs);
    printf("searching instructor: %s\n", instructor);
    printf("searching n keywords: %d\n", n_keywords);
    return 0;
}

int apply(char* plan) {
    // if plan exists
    return 0;
    // plan doesn't exist
}

int cbrowse(char subject[5], int number, char name[256], enum Attribute attrs[16], int n_keywords, char** keywords) {
    return 0;
}