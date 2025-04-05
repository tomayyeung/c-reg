/**
 * courses and sections methods
 */

#include <mongoc.h>

#include "commands.h"

#ifndef SECTIONS
#define SECTIONS

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

/**
 * Takes in course, eg CS221
 * returns a Course struct
 */
struct Course* course_from_section(HashTable* courses_map, const char* course_name);

#endif