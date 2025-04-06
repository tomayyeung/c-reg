#ifndef COMMANDS_HELPER
#define COMMANDS_HELPER

#include "sections.h"

void to_lower(char* s);
void to_upper(char* s);
void get_password(char* password, int max_len);
char* attr_to_str(enum Attribute a);
enum Attribute str_to_attr(char* s);
char* instr_mode_to_str(enum InstructionMode i);
enum InstructionMode str_to_instr_mode(char* s);
int* days_str_to_arr(const char* s);
void days_arr_to_str(char* buf, int* a);
void display_section(struct Section* s);
void display_sections(int n_sections, struct Section** sections);
int add_to_section(int crn, mongoc_collection_t* sections_collection, int addition);
int crn_exists(int crn, char* username, char* plan, mongoc_collection_t* plans_collection);
struct Section* crn_to_section(int crn, mongoc_collection_t* sections_collection, HashTable* courses_map);
int compare_section_times(const void *a, const void *b);

#endif
