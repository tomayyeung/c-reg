#include <stdio.h>
#include <string.h>
#include <bson.h>
#include <mongoc.h>

#include "commands.h"
#include "commands_helper.h"
#include "sections.h"
#include "user.h"


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

            // printf("saving username\n");
            
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

    // printf("login in commands.c\n");
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

int rm(int crn, char* plan, mongoc_collection_t* collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }
    
    bson_t *query = bson_new();
    bson_t *update = bson_new();
    bson_t child;
    bson_error_t error;

    // Build query: { name: "thomas" }
    BSON_APPEND_UTF8(query, "name", username);

    // Build update: { $pull: { "plans.main": 67890 } }
    char field_path[128];
    snprintf(field_path, sizeof(field_path), "plans.%s", plan);

    BSON_APPEND_DOCUMENT_BEGIN(update, "$pull", &child);
    BSON_APPEND_INT32(&child, field_path, crn);
    bson_append_document_end(update, &child);

    // Perform the update
    bool success = mongoc_collection_update_one(
        collection, query, update, NULL, NULL, &error);

    bson_destroy(query);
    bson_destroy(update);
    return success ? 0 : 1;
}

int browse(char subject[5], char number[16], enum InstructionMode instruction_mode_search, int n_attrs, enum Attribute attrs[16], char instructor[256], int n_keywords, char** keywords, mongoc_collection_t* sections_collection, mongoc_collection_t* courses_collection) {
    // printf("start isntructon mode: %d\n", instruction_mode_search);
    // printf("browse subject: %s\n", subject);
    // printf("browse number: %s\n", number);
    HashTable* courses_map = init_courses(courses_collection);
    // printf("created hash table in browse\n");

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
        // printf("found course: %s%s\n", course->subject, course->number);
        // printf("mid isntructon mode: %d\n", instruction_mode_search);

        // search course number
        if (*number != 0) {
            // printf("course number %s\n", (course->number));
            if (strcmp(number, course->number) != 0) continue;
        }
        // printf("efghhere\n");

        // search instruction mode
        int iter_instruction_mode;
        if (bson_iter_init_find(&iter, reply, "instruction_mode") && BSON_ITER_HOLDS_INT32(&iter)) {
            int integer = bson_iter_int32(&iter);
            // printf("reading int: %d\n", integer);
            iter_instruction_mode = integer;
        }
        // printf("instruction mode right befiore check %d\n", instruction_mode_search);
        if (instruction_mode_search != 3) {
            // printf("instruciton mode if %d\n", instruction_mode_search);
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
        // printf("HERE instruction mode: %d\n", instruction_mode_search);
        if (*instructor != 0) {
            // printf("abcdhere\n");
            if (strcmp(instructor, instructor_name) != 0) continue;
        }
        // printf("passed instructionmode test %d\n", instruction_mode_search);

        // grab other fields
        int iter_section_num;
        if (bson_iter_init_find(&iter, reply, "section_num") && BSON_ITER_HOLDS_INT32(&iter)) iter_section_num = bson_iter_int32(&iter);
        const char* iter_sched_type;
        if (bson_iter_init_find(&iter, reply, "schedule_type") && BSON_ITER_HOLDS_UTF8(&iter)) iter_sched_type = bson_iter_utf8(&iter, NULL);
        int* iter_days;
        if (bson_iter_init_find(&iter, reply, "days") && BSON_ITER_HOLDS_UTF8(&iter)) {
            iter_days = days_str_to_arr(bson_iter_utf8(&iter, NULL));
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
        

        // printf("before creating s instruction mode %d\n", instruction_mode_search);
        // search keywords
        struct Section *s = (struct Section*) malloc(sizeof(struct Section));
        s->course = course;
        s->section_num = iter_section_num;
        strcpy(s->schedule_type, iter_sched_type);
        // printf("xyzhere\n");
        // printf("instruction mode %d\n", instruction_mode_search);
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

        // printf("end isntruciton mode %d\n", instruction_mode_search);


        sections[i++] = s;
    }
    // printf("finished browsing\n");

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
    // printf("searching instruction mode: %d\n", (instruction_mode_search));
    // printf("searching n attrs: %d\n", n_attrs);
    // printf("searching instructor: %s\n", instructor);
    // printf("searching n keywords: %d\n", n_keywords);

    return 0;
}

int apply(char* plan, mongoc_collection_t *collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }
    
    bson_t *query, *update;
    const bson_t *doc;
    mongoc_cursor_t *cursor;
    bson_iter_t iter, plans_iter;
    bson_t child;
    bson_error_t error;
    char full_src_path[128];
    char* full_dest_path = "plans.main";

    snprintf(full_src_path, sizeof(full_src_path), "plans.%s", plan);

    query = BCON_NEW("name", BCON_UTF8(username));
    cursor = mongoc_collection_find_with_opts(collection, query, NULL, NULL);

    if (!mongoc_cursor_next(cursor, &doc)) {
        fprintf(stderr, "No document found.\n");
        bson_destroy(query);
        mongoc_cursor_destroy(cursor);
        return 1;
    }

    // Look for source array
    if (!(bson_iter_init_find(&iter, doc, "plans") &&
          BSON_ITER_HOLDS_DOCUMENT(&iter) &&
          bson_iter_recurse(&iter, &plans_iter))) {
        fprintf(stderr, "'plans' field missing or invalid.\n");
        bson_destroy(query);
        mongoc_cursor_destroy(cursor);
        return 1;
    }

    bson_iter_t array_iter;
    const uint8_t *raw_array = NULL;
    uint32_t raw_array_len = 0;

    while (bson_iter_next(&plans_iter)) {
        if (strcmp(bson_iter_key(&plans_iter), plan) == 0 &&
            BSON_ITER_HOLDS_ARRAY(&plans_iter)) {
            bson_iter_array(&plans_iter, &raw_array_len, &raw_array);
            break;
        }
    }

    if (!raw_array) {
        fprintf(stderr, "Source array '%s' not found.\n", plan);
        bson_destroy(query);
        mongoc_cursor_destroy(cursor);
        return 1;
    }

    // Construct the update
    update = bson_new();
    BSON_APPEND_DOCUMENT_BEGIN(update, "$set", &child);
    bson_t array_doc;
    if (bson_init_static(&array_doc, raw_array, raw_array_len)) {
        bson_append_array(&child, full_dest_path, -1, &array_doc);
        bson_destroy(&array_doc);  // optional but safe
    } else {
        fprintf(stderr, "Failed to initialize array from raw data.\n");
    }
    bson_append_document_end(update, &child);

    if (!mongoc_collection_update_one(collection, query, update, NULL, NULL, &error)) {
        fprintf(stderr, "Update failed: %s\n", error.message);
        bson_destroy(query);
        bson_destroy(update);
        mongoc_cursor_destroy(cursor);
        return 1;
    }

    bson_destroy(query);
    bson_destroy(update);
    mongoc_cursor_destroy(cursor);

    return 0;
}

int cbrowse(char subject[5], int number, char name[256], enum Attribute attrs[16], int n_keywords, char** keywords) {
    return 0;
}