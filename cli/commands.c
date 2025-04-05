#include <stdio.h>

#include "commands.h"

int login(char* user) {
    printf("login in commands.c\n");
    return 0;
}

int add(int crn, char* plan) {
    printf("added crn: %d to plan: %s\n", crn, plan);
    return 0;
}

int browse(char subject[5], int number, enum InstructionMode instruction_mode, char campus[256], char level[256], char attrs[256], char instructor[256]) {
    printf("searching subject: %s\n", subject);
    return 0;
}