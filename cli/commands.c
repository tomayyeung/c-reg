#include <stdio.h>
#include <string.h>
#include <bson.h>

#include "commands.h"
#include "commands_helper.h"
#include "user.h"

int art() {
    FILE *file = fopen("ascii.txt", "r");  // Open the file in read mode
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    char line[256];  // Buffer to store each line
    while (fgets(line, sizeof(line), file)) {  // Read each line
        printf("%s", line);  // Print the line
    }

    fclose(file);  // Close the file
    return 0;
}

int login(char* user, mongoc_collection_t* users_collection) {
    // Create a filter BSON document (empty filter means all documents)
    bson_t *filter = bson_new();  // Empty filter to fetch all documents
    const bson_t *reply;
    bson_error_t error;

    // Perform the find operation
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(users_collection, filter, NULL, NULL);

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

            save_username(user);
        }
    }

    if (mongoc_cursor_error(cursor, &error)) {
        fprintf(stderr, "Cursor error: %s\n", error.message);
    }

    // Cleanup
    bson_destroy(filter);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(users_collection);
    mongoc_cleanup();

    return 0;
}

int logout() {
    return remove(get_user_file_path());
}

int add(int crn, char* plan, mongoc_collection_t* plans_collection, mongoc_collection_t* sections_collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }

    char working_plan[256];
    if (*plan == 0) strcpy(working_plan, "main");
    else strcpy(working_plan, plan);

    bson_t *query, *update, child;
    bson_error_t error;

    // check if plan already contains crn
    if (crn_exists(crn, username, working_plan, plans_collection)) {
        fprintf(stderr, "CRN %d aready exists in plan %s\n", crn, working_plan);
        return 1;
    }

    // update db if working on main
    if (*plan == 0) {
        // find section, update enrollment
        printf("working main\n");
        if (add_to_section(crn, sections_collection, 1)) {
            return 1;
        }
    }

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
    if (!mongoc_collection_update_one(plans_collection, query, update, NULL, NULL, &error)) {
        fprintf(stderr, "Addition to plan failed: %s\n", error.message);
        bson_destroy(query);
        bson_destroy(update);
        return 1;
    }

    printf("Added crn: %d to plan: %s\n", crn, working_plan);
    bson_destroy(query);
    bson_destroy(update);
    return 0;
}

int rm(int crn, char* plan, mongoc_collection_t* plans_collection, mongoc_collection_t* sections_collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }

    char working_plan[256];
    if (*plan == 0) {
        strcpy(working_plan, "main");
    }
    else strcpy(working_plan, plan);

    // check if plan doesn't contains crn
    if (!crn_exists(crn, username, working_plan, plans_collection)) {
        fprintf(stderr, "CRN %d does not exist in plan %s\n", crn, working_plan);
        return 1;
    }

    // update db if working on main
    if (*plan == 0) {
        // find section, update enrollment
        printf("working main\n");
        if (add_to_section(crn, sections_collection, -1)) {
            return 1;
        }
    }
    
    bson_t *query = bson_new();
    bson_t *update = bson_new();
    bson_t child;
    bson_error_t error;

    // Build query: { name: "thomas" }
    BSON_APPEND_UTF8(query, "name", username);

    // Build update: { $pull: { "plans.main": 67890 } }
    char field_path[128];
    snprintf(field_path, sizeof(field_path), "plans.%s", working_plan);

    BSON_APPEND_DOCUMENT_BEGIN(update, "$pull", &child);
    BSON_APPEND_INT32(&child, field_path, crn);
    bson_append_document_end(update, &child);

    // Perform the update
    bool success = mongoc_collection_update_one(plans_collection, query, update, NULL, NULL, &error);

    printf("Removed crn: %d from plan: %s\n", crn, working_plan);
    bson_destroy(query);
    bson_destroy(update);
    return success ? 0 : 1;
}

int rmplan(const char *plan, mongoc_collection_t* plans_collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }

    bson_t *query = bson_new();
    bson_t *update = bson_new();
    bson_t child;
    bson_error_t error;

    // Build query: { "name": "thomas" }
    BSON_APPEND_UTF8(query, "name", username);

    // Build update: { $unset: { "plans.main": "" } }
    char field_path[128];
    snprintf(field_path, sizeof(field_path), "plans.%s", plan);

    BSON_APPEND_DOCUMENT_BEGIN(update, "$unset", &child);
    BSON_APPEND_UTF8(&child, field_path, "");  // Value doesn't matter
    bson_append_document_end(update, &child);

    // Run the update
    bool success = mongoc_collection_update_one(
        plans_collection, query, update, NULL, NULL, &error
    );

    if (!success) {
        fprintf(stderr, "Failed to remove plan '%s': %s\n", plan, error.message);
    }

    bson_destroy(query);
    bson_destroy(update);
    return success ? 0 : 1;
}

int view(char* plan, mongoc_collection_t* plans_collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }

    char working_plan[256];
    if (*plan == 0) strcpy(working_plan, "main");
    else strcpy(working_plan, plan);

    bson_t *query = bson_new();

    // Query to search for the plan document by name
    BSON_APPEND_UTF8(query, "name", username);

    // Perform the query to find the document
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(plans_collection, query, NULL, NULL);
    const bson_t *doc;

    if (!mongoc_cursor_next(cursor, &doc)) {
        fprintf(stderr, "No plans found for user: %s\n", username);
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // Access the "plans" sub-document
    bson_iter_t iter;
    if (!bson_iter_init_find(&iter, doc, "plans") || !BSON_ITER_HOLDS_DOCUMENT(&iter)) {
        fprintf(stderr, "Plans field not found or not a document.\n");
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // Extract the "plans" sub-document using bson_iter_document()
    const uint8_t *data;
    uint32_t length;
    bson_iter_document(&iter, &length, &data);  // Extract document data and length

    // Initialize a bson_t from the extracted data
    bson_t plans_doc;
    bson_init_static(&plans_doc, data, length);

    // Access the array inside the "plans" sub-document
    if (!bson_iter_init_find(&iter, &plans_doc, working_plan) || !BSON_ITER_HOLDS_ARRAY(&iter)) {
        fprintf(stderr, "Array not found or not an array.\n");
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // Iterate through the array elements
    bson_iter_t array_iter;
    if (BSON_ITER_HOLDS_ARRAY(&iter)) {
        bson_iter_recurse(&iter, &array_iter);  // Initialize an array iterator
        
        // Loop through the array
        printf("Plan '%s':\n", working_plan);
        while (bson_iter_next(&array_iter)) {
            if (BSON_ITER_HOLDS_INT32(&array_iter)) {
                printf(" - %d\n", bson_iter_int32(&array_iter));
            }
        }
    } else {
        fprintf(stderr, "Main array is not of array type.\n");
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // cleanup
    mongoc_cursor_destroy(cursor);
    bson_destroy(query);
    return 0;
}

int viewplans(mongoc_collection_t* plans_collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }

    bson_t *query;
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_iter_t iter, plans_iter;
    bson_error_t error;

    // Query: { name: "thomas" }
    query = BCON_NEW("name", BCON_UTF8(username));
    cursor = mongoc_collection_find_with_opts(plans_collection, query, NULL, NULL);

    if (!mongoc_cursor_next(cursor, &doc)) {
        fprintf(stderr, "User '%s' not found or no plans.\n", username);
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // Navigate to "plans"
    if (!(bson_iter_init_find(&iter, doc, "plans") && BSON_ITER_HOLDS_DOCUMENT(&iter) && bson_iter_recurse(&iter, &plans_iter))) {
        fprintf(stderr, "'plans' field missing or invalid.\n");
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // Iterate and print all plan keys;
    while (bson_iter_next(&plans_iter)) {
        const char *plan_name = bson_iter_key(&plans_iter);
        printf(" - %s\n", plan_name);
    }

    mongoc_cursor_destroy(cursor);
    bson_destroy(query);
    return 0;
}

int browse(char subject[5], char number[16], enum InstructionMode instruction_mode_search, int n_attrs, enum Attribute attrs[16], char instructor[256], int n_keywords, char** keywords, mongoc_collection_t* sections_collection, mongoc_collection_t* courses_collection) {
    to_upper(subject);

    HashTable* courses_map = init_courses(courses_collection);

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
        if (i >= MAX_BROWSE) break;

        bson_iter_t iter;

        // create course from section to compare subject/course num
        const char* iter_course;
        if (bson_iter_init_find(&iter, reply, "course") && BSON_ITER_HOLDS_UTF8(&iter)) {
            iter_course = bson_iter_utf8(&iter, NULL);
        }

        struct Course* course = course_from_section(courses_map, iter_course);
        if (course == 0) {
            continue;
        }

        // search subject
        if (*subject != 0) {
            if (strcmp(subject, course->subject) != 0) {
                continue;
            }
        }

        // search course number
        if (*number != 0) {
            if (strcmp(number, course->number) != 0) continue;
        }

        // search instruction mode
        int iter_instruction_mode;
        if (bson_iter_init_find(&iter, reply, "instruction_mode") && BSON_ITER_HOLDS_INT32(&iter)) iter_instruction_mode = bson_iter_int32(&iter);

        if (instruction_mode_search != 3) {
            if (instruction_mode_search != iter_instruction_mode) continue;
        }

        // search attributes

        // search instructor
        // build instructor name
        const char* iter_instructor_first;
        if (bson_iter_init_find(&iter, reply, "instructor_first") && BSON_ITER_HOLDS_UTF8(&iter)) iter_instructor_first = bson_iter_utf8(&iter, NULL);
        const char* iter_instructor_last;
        if (bson_iter_init_find(&iter, reply, "instructor_last") && BSON_ITER_HOLDS_UTF8(&iter)) iter_instructor_last = bson_iter_utf8(&iter, NULL);

        char instructor_name[256];
        sprintf(instructor_name, "%s %s", iter_instructor_first, iter_instructor_last);
        if (*instructor != 0) {
            if (strcmp(instructor, instructor_name) != 0) continue;
        }

        // all tests passed - add this section

        // grab crn
        int iter_crn;
        if (bson_iter_init_find(&iter, reply, "crn") && BSON_ITER_HOLDS_INT32(&iter)) iter_crn = bson_iter_int32(&iter);
        struct Section *s = crn_to_section(iter_crn, sections_collection, courses_map);

        sections[i++] = s;
    }

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

    return 0;
}

int apply(char* plan, mongoc_collection_t* plans_collection) {
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
    cursor = mongoc_collection_find_with_opts(plans_collection, query, NULL, NULL);

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

    if (!mongoc_collection_update_one(plans_collection, query, update, NULL, NULL, &error)) {
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

int schedule(char* plan, mongoc_collection_t* courses_collection, mongoc_collection_t* plans_collection, mongoc_collection_t* sections_collection) {
    char* username = load_username();
    if (username == NULL) {
        fprintf(stderr, "Not logged in\n");
        return 1;
    }

    char working_plan[256];
    if (*plan == 0) strcpy(working_plan, "main");
    else strcpy(working_plan, plan);

    HashTable* courses_map = init_courses(courses_collection);
    struct Section* sections[16];
    int sectionsI = 0;

    bson_t *query = bson_new();
    bson_error_t error;

    // Query to search for the plan document by name
    BSON_APPEND_UTF8(query, "name", username);

    // Perform the query to find the document
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(plans_collection, query, NULL, NULL);
    const bson_t *doc;

    if (!mongoc_cursor_next(cursor, &doc)) {
        fprintf(stderr, "No plans found for user: %s\n", username);
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // Access the "plans" sub-document
    bson_iter_t iter;
    if (!bson_iter_init_find(&iter, doc, "plans") || !BSON_ITER_HOLDS_DOCUMENT(&iter)) {
        fprintf(stderr, "Plans field not found or not a document.\n");
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // Extract the "plans" sub-document using bson_iter_document()
    const uint8_t *data;
    uint32_t length;
    bson_iter_document(&iter, &length, &data);  // Extract document data and length

    // Initialize a bson_t from the extracted data
    bson_t plans_doc;
    bson_init_static(&plans_doc, data, length);

    // Access the array inside the "plans" sub-document
    if (!bson_iter_init_find(&iter, &plans_doc, working_plan) || !BSON_ITER_HOLDS_ARRAY(&iter)) {
        fprintf(stderr, "Array not found or not an array.\n");
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // Iterate through the array elements
    bson_iter_t array_iter;
    if (BSON_ITER_HOLDS_ARRAY(&iter)) {
        bson_iter_recurse(&iter, &array_iter);  // Initialize an array iterator
        
        // Loop through the array
        while (bson_iter_next(&array_iter)) {
            if (BSON_ITER_HOLDS_INT32(&array_iter)) {
                sections[sectionsI++] = crn_to_section(bson_iter_int32(&array_iter), sections_collection, courses_map);
            }
        }
    } else {
        fprintf(stderr, "Main array is not of array type.\n");
        mongoc_cursor_destroy(cursor);
        bson_destroy(query);
        return 1;
    }

    // cleanup
    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(courses_collection);
    mongoc_collection_destroy(plans_collection);
    mongoc_collection_destroy(sections_collection);
    mongoc_cleanup();

    // sort sections for each day
    int num_sections_by_day[7] = {0, 0, 0, 0, 0, 0, 0};
    struct Section* sections_by_day[7][16];
    for (int i = 0; i < 7; i++) { // i: day
        for (int j = 0; j < sectionsI; j++) { // j: index of section
            struct Section* s = sections[j];

            if (s->days[i]) {
                if (num_sections_by_day[i] >= 16) {
                    fprintf(stderr, "Cannot display this many sections in schedule\n");
                    return 1;
                }
                sections_by_day[i][num_sections_by_day[i]] = s;
                num_sections_by_day[i] += 1;
            }
        }

        qsort(sections_by_day[i], num_sections_by_day[i], sizeof(struct Section*), compare_section_times);
    }

    char* days[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    for (int i = 0; i < 7; i++) { // i: day
        printf("%s:\n", days[i]);

        for (int j = 0; j < num_sections_by_day[i]; j++) { // j: index of section
            display_section(sections_by_day[i][j]);
        }

        printf("\n");
    }

    return 0;
}
