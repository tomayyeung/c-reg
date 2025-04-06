#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sections.h"

#define TABLE_SIZE 10007  // A prime number larger than 5000, helps with uniform distribution

// Hash function (djb2 by Daniel J. Bernstein)
unsigned int hash(const char *str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash % TABLE_SIZE;  // Use modulo to fit the hash into the table size
}

// Create a new hash table
HashTable* create_hash_table() {
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    ht->table = (Node **)malloc(TABLE_SIZE * sizeof(Node *));
    
    // Initialize all table entries to NULL
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->table[i] = NULL;
    }

    return ht;
}

// Insert a key into the hash table
void insert(HashTable *ht, const char *key, struct Course *course) {
    unsigned int index = hash(key);
    
    // Create a new node for the key
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->key = strdup(key);
    new_node->course = course;
    new_node->next = ht->table[index];  // Insert at the beginning of the list
    
    // Update the hash table index
    ht->table[index] = new_node;
}

// Search for a key in the hash table
struct Course* search(HashTable *ht, const char *key) {
    unsigned int index = hash(key);
    
    Node *current = ht->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->course;  // Found the key
        }
        current = current->next;
    }

    return 0;  // Key not found
}

// Delete a key from the hash table
void delete(HashTable *ht, char *key) {
    unsigned int index = hash(key);
    
    Node *current = ht->table[index];
    Node *prev = NULL;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                ht->table[index] = current->next;
            }
            free(current->key);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Free the entire hash table
void free_hash_table(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *current = ht->table[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp->key);
            free(temp->course);
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}

HashTable* init_courses(mongoc_collection_t* courses_collection) {
    HashTable* courses_map = create_hash_table();

    // Create a filter BSON document (empty filter means all documents)
    bson_t *filter = bson_new();  // Empty filter to fetch all documents
    const bson_t *reply;
    bson_error_t error;

    // Perform the find operation
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(courses_collection, filter, NULL, NULL);

    // Iterate over the cursor
    while (mongoc_cursor_next(cursor, &reply)) {
        bson_iter_t iter;
        struct Course* c = create_course_from_cursor(iter, reply);
        if (c == 0) continue;
        
        char key[16];
        sprintf(key, "%s%s", c->subject, c->number);
        insert(courses_map, key, c);
    }

    return courses_map;
}

struct Course* create_course_from_cursor(bson_iter_t iter, const bson_t *reply) {
    const char* iter_subj;
    if (bson_iter_init_find(&iter, reply, "subject") && BSON_ITER_HOLDS_UTF8(&iter)) iter_subj = bson_iter_utf8(&iter, NULL);
    const char* iter_num;
    if (bson_iter_init_find(&iter, reply, "number") && BSON_ITER_HOLDS_UTF8(&iter)) iter_num = bson_iter_utf8(&iter, NULL);
    const char* iter_name;
    if (bson_iter_init_find(&iter, reply, "course_title") && BSON_ITER_HOLDS_UTF8(&iter)) iter_name = bson_iter_utf8(&iter, NULL);
    const char* iter_units;
    if (bson_iter_init_find(&iter, reply, "units") && BSON_ITER_HOLDS_UTF8(&iter)) iter_units = bson_iter_utf8(&iter, NULL);
    const char* iter_descr;
    if (bson_iter_init_find(&iter, reply, "description") && BSON_ITER_HOLDS_UTF8(&iter)) iter_descr = bson_iter_utf8(&iter, NULL);
    const char* iter_prereq;
    if (bson_iter_init_find(&iter, reply, "prerequisites") && BSON_ITER_HOLDS_UTF8(&iter)) iter_prereq = bson_iter_utf8(&iter, NULL);
    const char* iter_restr;
    if (bson_iter_init_find(&iter, reply, "restrictions") && BSON_ITER_HOLDS_UTF8(&iter)) iter_restr = bson_iter_utf8(&iter, NULL);
    const char* iter_college;
    if (bson_iter_init_find(&iter, reply, "college") && BSON_ITER_HOLDS_UTF8(&iter)) iter_college = bson_iter_utf8(&iter, NULL);
    
    struct Course* c = (struct Course*) malloc(sizeof(struct Course));
    c->subject = iter_subj;
    c->number = iter_num;
    c->name = iter_name;
    c->units = iter_units;
    c->descr = iter_descr;
    c->prereq = iter_prereq;
    c->restr = iter_restr;
    c->college = iter_college;

    if (*iter_subj == 0) return 0; // bit of error handling
    return c;
}

struct Course* course_from_section(HashTable* courses_map, const char* course_name) {
    return search(courses_map, course_name);
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
    }

    bson_destroy(query);
    bson_destroy(update);
    return success ? 0 : 1;
}

int crn_exists(int crn, char* username, char* plan, mongoc_collection_t* plans_collection) {
    bson_t *query = bson_new();
    bson_t *filter = bson_new();

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

struct Section* crn_to_section(int crn, mongoc_collection_t* sections_collection, HashTable* courses_map) {
    bson_t *query = bson_new();
    bson_t *filter = bson_new();

    // Query to check if the crn exists in the plans array
    BSON_APPEND_INT32(query, "crn", crn);

    // Perform the query
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(sections_collection, query, NULL, NULL);
    const bson_t *reply;

    while (mongoc_cursor_next(cursor, &reply)) {
        bson_iter_t iter;

        const char* iter_course = NULL;
        if (bson_iter_init_find(&iter, reply, "course") && BSON_ITER_HOLDS_UTF8(&iter)) iter_course = bson_iter_utf8(&iter, NULL);
        struct Course* c = course_from_section(courses_map, iter_course);

        int iter_section_num;
        if (bson_iter_init_find(&iter, reply, "section_num") && BSON_ITER_HOLDS_INT32(&iter)) iter_section_num = bson_iter_int32(&iter);
        int iter_crn;
        if (bson_iter_init_find(&iter, reply, "crn") && BSON_ITER_HOLDS_INT32(&iter)) iter_crn = bson_iter_int32(&iter);
        const char* iter_sched_type;
        if (bson_iter_init_find(&iter, reply, "schedule_type") && BSON_ITER_HOLDS_UTF8(&iter)) iter_sched_type = bson_iter_utf8(&iter, NULL);
        int iter_instruction_mode;
        if (bson_iter_init_find(&iter, reply, "instruction_mode") && BSON_ITER_HOLDS_INT32(&iter)) iter_instruction_mode = bson_iter_int32(&iter);
        int* iter_days;
        if (bson_iter_init_find(&iter, reply, "days") && BSON_ITER_HOLDS_UTF8(&iter)) iter_days = days_str_to_arr(bson_iter_utf8(&iter, NULL));
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
        const char* iter_instructor_first;
        if (bson_iter_init_find(&iter, reply, "instructor_first") && BSON_ITER_HOLDS_UTF8(&iter)) iter_instructor_first = bson_iter_utf8(&iter, NULL);
        const char* iter_instructor_last;
        if (bson_iter_init_find(&iter, reply, "instructor_last") && BSON_ITER_HOLDS_UTF8(&iter)) iter_instructor_last = bson_iter_utf8(&iter, NULL);
        if (bson_iter_init_find(&iter, reply, "enrollment") && BSON_ITER_HOLDS_INT32(&iter)) iter_enrollment = bson_iter_int32(&iter);
        const char* iter_instructor_email;
        if (bson_iter_init_find(&iter, reply, "instructor_email") && BSON_ITER_HOLDS_UTF8(&iter)) iter_instructor_email = bson_iter_utf8(&iter, NULL);
        const char* iter_college;
        if (bson_iter_init_find(&iter, reply, "college") && BSON_ITER_HOLDS_UTF8(&iter)) iter_college = bson_iter_utf8(&iter, NULL);
        
        // search keywords
        struct Section *s = (struct Section*) malloc(sizeof(struct Section));
        s->course = c;
        s->section_num = iter_section_num;
        s->crn = iter_crn;
        strcpy(s->schedule_type, iter_sched_type);
        s->instruction_mode = (enum InstructionMode)(iter_instruction_mode);
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
        
        return s;
    }

    return 0;
}

int compare_section_times(const void *a, const void *b) {
    const struct Section* s1 = *(const struct Section**)a;
    const struct Section* s2 = *(const struct Section**)b;

    if (s1->begin_time < s2->begin_time) return -1;
    if (s1->begin_time > s2->begin_time) return 1;
    if (s1->end_time < s2->end_time) return -1;
    if (s1->end_time > s2->end_time) return 1;
    return 0;
}
