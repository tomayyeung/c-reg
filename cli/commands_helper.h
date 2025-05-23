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
void display_section(struct Section* s, int verbose);
void display_sections(int n_sections, struct Section** sections, int verbose);
void display_course(struct Course* c, int verbose);
void display_courses(int n_courses, struct Course** courses, int verbose);

#endif
