/**
 * courses and sections methods
 */

#ifndef SECTIONS
#define SECTIONS

#include <mongoc.h>

#include "commands.h"
#include "commands_helper.h"

typedef struct Node {
    char *key;
    struct Course* course;
    struct Node *next;
} Node;

typedef struct HashTable {
    Node **table;  // Array of pointers to nodes
} HashTable;

unsigned int hash(const char *str);
HashTable* create_hash_table();
void insert(HashTable *ht, const char *key, struct Course *course);
struct Course* search(HashTable *ht, const char *key);
void delete(HashTable *ht, char *key);
void free_hash_table(HashTable *ht);
HashTable* init_courses(mongoc_collection_t* courses_collection);
struct Course* create_course_from_cursor(bson_iter_t iter, const bson_t *reply);
struct Course* course_from_section(HashTable* courses_map, const char* course_name);
int add_to_section(int crn, mongoc_collection_t* sections_collection, int addition);
int crn_exists(int crn, char* username, char* plan, mongoc_collection_t* plans_collection);
struct Section* crn_to_section(int crn, mongoc_collection_t* sections_collection, HashTable* courses_map);
int compare_section_times(const void *a, const void *b);

#endif
