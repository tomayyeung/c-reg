#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <bson.h>
#include <mongoc.h>

#include "commands.h"
#include "sections.h"
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
    if (strcmp(s, "hybrid") == 0) return Hybrid;
    else if (strcmp(s, "inperson") == 0) return InPerson;
    else if (strcmp(s, "online") == 0) return Online;
    return -1;
}

int login(char* user, mongoc_collection_t* collection) {
    // Create a filter BSON document (empty filter means all documents)
    bson_t *filter = bson_new();  // Empty filter to fetch all documents
    const bson_t *reply;
    bson_error_t error;

    // Perform the find operation
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);

    // Iterate over the cursor
    while (mongoc_cursor_next(cursor, &reply)) {
        bson_iter_t iter;

        // get password
        const char* password;
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

int add(int crn, char* plan, mongoc_collection_t* collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }

    bson_t *query, *update, child;
    bson_error_t error;

    char working_plan[256];
    if (*plan == 0) strcpy(working_plan, "main");
    else strcpy(working_plan, plan);

    // Build the field path: plans.<plan_name>
    char field_path[128];
    snprintf(field_path, sizeof(field_path), "plans.%s", working_plan);

    // Find the document with matching name
    query = BCON_NEW("name", BCON_UTF8(username));

    // Build the update: $push: { "plans.<plan_name>": new_id }
    update = bson_new();
    BSON_APPEND_DOCUMENT_BEGIN(update, "$push", &child);
    BSON_APPEND_INT32(&child, field_path, crn);
    bson_append_document_end(update, &child);

    // Execute update
    if (!mongoc_collection_update_one(collection, query, update, NULL, NULL, &error)) {
        fprintf(stderr, "Addition to plan failed: %s\n", error.message);
        bson_destroy(query);
        bson_destroy(update);
        return 1;
    }

    printf("added crn: %d to plan: %s\n", crn, plan);
    bson_destroy(query);
    bson_destroy(update);
    return 0;
}

void display_sections(int n_sections, struct Section** sections) {
    printf("nsections: %d\n", n_sections);
    for (int i = 0; i < n_sections; i++) {
        struct Course* c = sections[i]->course;
        printf("Course: %s%s: %s\n", c->subject, c->number, c->name);
    }
}

int browse(char subject[5], char* number, enum InstructionMode instruction_mode_search, int n_attrs, enum Attribute attrs[16], char instructor[256], int n_keywords, char** keywords, mongoc_collection_t* sections_collection, mongoc_collection_t* courses_collection) {
    printf("start isntructon mode: %d\n", instruction_mode_search);
    printf("browse subject: %s\n", subject);
    HashTable* courses_map = init_courses(courses_collection);
    printf("created hash table in browse\n");

    struct Section* sections[MAX_BROWSE];
    int i = 0;

    // Create a filter BSON document (empty filter means all documents)
    bson_t *filter = bson_new();  // Empty filter to fetch all documents
    const bson_t *reply;
    bson_error_t error;

    // Perform the find operation
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(sections_collection, filter, NULL, NULL);

    // Iterate over the cursor
    while (mongoc_cursor_next(cursor, &reply)) {
        // printf("loop\n");
        if (i >= MAX_BROWSE) break;

        bson_iter_t iter;

        // create course from section
        const char* iter_course;
        if (bson_iter_init_find(&iter, reply, "course") && BSON_ITER_HOLDS_UTF8(&iter)) {
            iter_course = bson_iter_utf8(&iter, NULL);
        }
        // printf("course: %s\n", iter_course);
        struct Course* course = course_from_section(courses_map, iter_course);
        if (course == 0) {
            continue;
        }
        // printf("searching for subject: %s\n", subject);
        // printf("current subject: %s\n", course->subject);

        // search subject
        if (*subject != 0) {
            // printf("in if\n");
            // printf("course name: %s\n", course->name);
            // printf("course subject: %s\n", course->subject);
            if (strcmp(subject, course->subject) != 0) {
                // printf("skipped - subject: %s course->subject: %s\n", subject, course->subject);
                continue;
            }
        }
        printf("found course: %s%s\n", course->subject, course->number);
        // printf("mid isntructon mode: %d\n", instruction_mode_search);

        // search course number
        if (number != 0) {
            printf("course number %s\n", (course->number));
            if (strcmp(number, course->number) != 0) continue;
        }
        // printf("efghhere\n");

        // search instruction mode
        int iter_instruction_mode;
        if (bson_iter_init_find(&iter, reply, "instruction_mode") && BSON_ITER_HOLDS_INT32(&iter)) {
            int integer = bson_iter_int32(&iter);
            printf("reading int: %d\n", integer);
            iter_instruction_mode = integer;
        }
        // printf("instruction mode right befiore check %d\n", instruction_mode_search);
        if (instruction_mode_search != 3) {
            printf("instruciton mode if %d\n", instruction_mode_search);
            if (instruction_mode_search != iter_instruction_mode) continue;
        }

        // search attributes

        // search instructor
        const char* iter_instructor_first;
        if (bson_iter_init_find(&iter, reply, "instructor_first") && BSON_ITER_HOLDS_UTF8(&iter)) {
            iter_instructor_first = bson_iter_utf8(&iter, NULL);
        }
        const char* iter_instructor_last;
        if (bson_iter_init_find(&iter, reply, "instructor_last") && BSON_ITER_HOLDS_UTF8(&iter)) {
            iter_instructor_last = bson_iter_utf8(&iter, NULL);
        }
        char instructor_name[256];
        sprintf(instructor_name, "%s %s", iter_instructor_first, iter_instructor_last);
        printf("HERE instruction mode: %d\n", instruction_mode_search);
        if (*instructor != 0) {
            // printf("abcdhere\n");
            if (strcmp(instructor, instructor_name) != 0) continue;
        }
        printf("passed instructionmode test %d\n", instruction_mode_search);

        // grab other fields
        int iter_section_num;
        if (bson_iter_init_find(&iter, reply, "instructor_first") && BSON_ITER_HOLDS_INT32(&iter)) iter_section_num = bson_iter_int32(&iter);
        const char* iter_sched_type;
        if (bson_iter_init_find(&iter, reply, "schedule_type") && BSON_ITER_HOLDS_UTF8(&iter)) iter_sched_type = bson_iter_utf8(&iter, NULL);
        int iter_days[7];
        if (bson_iter_init_find(&iter, reply, "days") && BSON_ITER_HOLDS_UTF8(&iter)) {
            // parse days
        }
        int iter_begin_time;
        if (bson_iter_init_find(&iter, reply, "begin_time") && BSON_ITER_HOLDS_INT32(&iter)) iter_begin_time = bson_iter_int32(&iter);
        int iter_end_time;
        if (bson_iter_init_find(&iter, reply, "end_time") && BSON_ITER_HOLDS_INT32(&iter)) iter_end_time = bson_iter_int32(&iter);
        const char* iter_start_date;
        if (bson_iter_init_find(&iter, reply, "start_date") && BSON_ITER_HOLDS_UTF8(&iter)) iter_start_date = bson_iter_utf8(&iter, NULL);
        const char* iter_end_date;
        if (bson_iter_init_find(&iter, reply, "end_date") && BSON_ITER_HOLDS_UTF8(&iter)) iter_end_date = bson_iter_utf8(&iter, NULL);
        const char* iter_building;
        if (bson_iter_init_find(&iter, reply, "building") && BSON_ITER_HOLDS_UTF8(&iter)) iter_building = bson_iter_utf8(&iter, NULL);
        const char* iter_room;
        if (bson_iter_init_find(&iter, reply, "room") && BSON_ITER_HOLDS_UTF8(&iter)) iter_room = bson_iter_utf8(&iter, NULL);
        int iter_capacity;
        if (bson_iter_init_find(&iter, reply, "capacity") && BSON_ITER_HOLDS_INT32(&iter)) iter_capacity = bson_iter_int32(&iter);
        int iter_enrollment;
        if (bson_iter_init_find(&iter, reply, "enrollment") && BSON_ITER_HOLDS_INT32(&iter)) iter_enrollment = bson_iter_int32(&iter);
        const char* iter_instructor_email;
        if (bson_iter_init_find(&iter, reply, "instructor_email") && BSON_ITER_HOLDS_UTF8(&iter)) iter_instructor_email = bson_iter_utf8(&iter, NULL);
        const char* iter_college;
        if (bson_iter_init_find(&iter, reply, "college") && BSON_ITER_HOLDS_UTF8(&iter)) iter_college = bson_iter_utf8(&iter, NULL);
        

        printf("before creating s instruction mode %d\n", instruction_mode_search);
        // search keywords
        struct Section *s = (struct Section*) malloc(sizeof(struct Section));
        s->course = course;
        s->section_num = iter_section_num;
        strcpy(s->schedule_type, iter_sched_type);
        // printf("xyzhere\n");
        printf("instruction mode %d\n", instruction_mode_search);
        s->instruction_mode = (enum InstructionMode)(iter_instruction_mode);
        // s->instruction_mode = Hybrid;
        // printf("lmnophere\n");
        s->days = iter_days;
        s->begin_time = iter_begin_time;
        s->end_time = iter_end_time;
        strcpy(s->start_date, iter_start_date);
        strcpy(s->end_date, iter_end_date);
        strcpy(s->building, iter_building);
        strcpy(s->room, iter_room);
        s->capacity = iter_capacity;
        s->enrollment = iter_enrollment;
        strcpy(s->instructor_first, iter_instructor_first);
        strcpy(s->instructor_last, iter_instructor_last);
        strcpy(s->instructor_email, iter_instructor_email);
        strcpy(s->college, iter_college);

        printf("end isntruciton mode %d\n", instruction_mode_search);


        sections[i++] = s;
    }
    printf("finished browsing\n");

    if (mongoc_cursor_error(cursor, &error)) {
        fprintf(stderr, "Cursor error: %s\n", error.message);
        return 1;
    }

    display_sections(i, sections);

    // Cleanup
    bson_destroy(filter);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(sections_collection);
    mongoc_collection_destroy(courses_collection);
    mongoc_cleanup();
    
    // printf("searching subject: %s\n", subject);
    // printf("searching number: %d\n", number);
    printf("searching instruction mode: %d\n", (instruction_mode_search));
    // printf("searching n attrs: %d\n", n_attrs);
    // printf("searching instructor: %s\n", instructor);
    // printf("searching n keywords: %d\n", n_keywords);

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