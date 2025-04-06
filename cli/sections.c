#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mongoc.h>

#include "sections.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        // struct Course* c;

        // printf("shere\n");
        const char* subject;
        if (bson_iter_init_find(&iter, reply, "subject") && BSON_ITER_HOLDS_UTF8(&iter)) subject = bson_iter_utf8(&iter, NULL);
        // printf("found subject\n");
        const char* number;
        if (bson_iter_init_find(&iter, reply, "number") && BSON_ITER_HOLDS_UTF8(&iter)) number = bson_iter_utf8(&iter, NULL);
        const char* name;
        if (bson_iter_init_find(&iter, reply, "course_title") && BSON_ITER_HOLDS_UTF8(&iter)) name = bson_iter_utf8(&iter, NULL);
        // printf("subject: %s number: %s name: %s\n", subject, number, name);

        struct Course* c = (struct Course*) malloc(sizeof(struct Course));
        c->subject = subject;
        c->number = number;
        c->name = name;

        // printf("dhere\n");
        // strcpy(c->subject, subject);
        // strcpy(c->number, number);
        // strcpy(c->name, name);

        
        char key[16];
        // printf("init key\n");
        sprintf(key, "%s%s", subject, number);
        // printf("created key\n");
        insert(courses_map, key, c);
        // printf("inserted\n");
    }

    return courses_map;
}

struct Course* course_from_section(HashTable* courses_map, const char* course_name) {
    return search(courses_map, course_name);
}