#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "commands.h"
#include "commands_helper.h"

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
    return -1;
}

char* instr_mode_to_str(enum InstructionMode i) {
    switch (i) {
        case Hybrid: return "Hybrid";
        case InPerson: return "InPerson";
        case Online: return "Online";
        default: return 0;
    }
}

enum InstructionMode str_to_instr_mode(char* s) {
    to_lower(s);
    if (strcmp(s, "inperson") == 0) return InPerson;
    else if (strcmp(s, "online") == 0) return Online;
    else if (strcmp(s, "hybrid") == 0) return Hybrid;
    return -1;
}

int* days_str_to_arr(const char* s) {
    int* out = (int*) malloc(sizeof(int)*7);
    for (int i = 0; i < 7; i++) {
        out[i] = 0;
    }

    while (*s != 0) {
        switch (*s) {
            case 'M':
                out[0] = 1;
                break;
            case 'T':
                out[1] = 1;
                break;
            case 'W':
                out[2] = 1;
                break;
            case 'R':
                out[3] = 1;
                break;
            case 'F':
                out[4] = 1;
                break;
            case 'S':
                out[5] = 1;
                break;
        }
        s++;
    }

    return out;
}

void days_arr_to_str(char* buf, int* a) {
    for (int i = 0; i < 7; i++) {
        buf[i] = 0;
    }
    for (int i = 0; i < 7; i++) {
        if (a[i] == 0) continue;

        switch (i) {
            case 0:
                *(buf++) = 'M';
                break;
            case 1:
                *(buf++) = 'T';
                break;
            case 2:
                *(buf++) = 'W';
                break;
            case 3:
                *(buf++) = 'R';
                break;
            case 4:
                *(buf++) = 'F';
                break;
            case 5:
                *(buf++) = 'S';
                break;
        }
    }

}

void display_sections(int n_sections, struct Section** sections) {
    // printf("nsections: %d\n", n_sections);
    for (int i = 0; i < n_sections; i++) {
        struct Section* s = sections[i];
        struct Course* c = s->course;
        char days[8];
        days_arr_to_str(days, s->days);
        printf("%d %s%s-%d: %s %s %d-%d %s%s Mode: %s\n", 
            s->crn, c->subject, c->number, s->section_num, c->name, days, s->begin_time, s->end_time, s->building, s->room, instr_mode_to_str(s->instruction_mode));
    }
}

int add_to_section(int crn, mongoc_collection_t* sections_collection, int addition) {
    // addition = 1 for adding, -1 for removing
    bson_t *query = bson_new();
    bson_t *update = bson_new();
    bson_t set_doc;
    bson_error_t error;

    // Query: { "crn": crn, "capacity": { "$gte": 1} }
    BSON_APPEND_INT32(query, "crn", crn);
    BSON_APPEND_DOCUMENT_BEGIN(query, "capacity", &set_doc);
    BSON_APPEND_INT32(&set_doc, "$gte", 1);  // Greater than or equal to 1
    bson_append_document_end(query, &set_doc);

    // Update: { $inc: { "capacity": -addition }, $inc: { "enrollment": addition } }
    BSON_APPEND_DOCUMENT_BEGIN(update, "$inc", &set_doc);
    BSON_APPEND_INT32(&set_doc, "capacity", -addition);  // Decrement by 1
    bson_append_document_end(update, &set_doc);

    BSON_APPEND_DOCUMENT_BEGIN(update, "$inc", &set_doc);
    BSON_APPEND_INT32(&set_doc, "enrollment", addition);  // increment by 1
    bson_append_document_end(update, &set_doc);

    // Perform the update
    bool success = mongoc_collection_update_one(sections_collection, query, update, NULL, NULL, &error);
    
    if (!success) {
        fprintf(stderr, "Failed to decrement semester: %s\n", error.message);
    } else {
        printf("crn %d \n", crn);
    }

    bson_destroy(query);
    bson_destroy(update);
    return success ? 0 : 1;
}

int crn_exists(int crn, char* username, char* plan, mongoc_collection_t* plans_collection) {
    bson_t *query = bson_new();
    bson_t *filter = bson_new();

    // // Query to check if the specific plan contains the crn
    // BSON_APPEND_UTF8(query, "name", username);
    // char plan_field[256];
    // snprintf(plan_field, sizeof(plan_field), "plans.%s.crn", plan);  // Example: "plans.main.crn"
    // BSON_APPEND_INT32(query, plan_field, crn);  // Match crn in the specified plan

    // // Perform the query
    // mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(plans_collection, query, NULL, NULL);
    // const bson_t *doc;


    // Query to check if the crn exists in the plans array
    BSON_APPEND_UTF8(query, "name", username);
    char plan_field[256];
    snprintf(plan_field, sizeof(plan_field), "plans.%s", plan); // eg "plans.A"
    BSON_APPEND_INT32(query, plan_field, crn); // Match crn in the plans array

    // Perform the query
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(plans_collection, query, NULL, NULL);
    const bson_t *doc;
    
    int found = 0;
    while (mongoc_cursor_next(cursor, &doc)) {
        // If we find a document, it means the crn is in the plan
        found = 1;
        break;  // We found the crn in the plan, no need to continue
    }
    return found;
}