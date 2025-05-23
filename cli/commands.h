/**
 * Commands for creg cli
 */

#ifndef COMMANDS
#define COMMANDS

#include <mongoc.h>

#define MAX_BROWSE (1024)

/**
 * course attributes
 */
enum Attribute {
    ATTR_NULL_VALUE,
    AI, // Arrupe Immersion
    CLN, // Clinical Sites for Nursing
    A1,
    A2,
    B1,
    B2,
    C1,
    C2,
    CD,
    CEL,
    D1,
    D2,
    D3,
    E,
    F,
    SL, // Core: Service Learning
    ELS, // Education: Liberal Studies
    ENVA, // ENVA Pathway Courses
    FL2, // Foreign Langauge 2
    FL3, // Foreign Langauge 3
    FL4, // Foreign Langauge 4
    HCCP, // Honors College Curric Priority
    HMA, // Hybrid Modality Asynchronous
    PSYCH_CULT, // Psychology Cult Div Req
    THR1, // Christianity
    THR2, // Global Religions and Theo
    THR3, // Asian Religions
    THR4, // Relig, Ethics, Soc Justice
    TS, // Transfer Seminar
    // UG MJR ASIAN STUD ELECT (ANST)
    // UG MJR COMS ELECT (COMS)
    // UG MJR FNAR ELECT (FNAR)
    // UG MJR GRDE ELECT (GRDE)
    // UG MJR JAPN ELECT (JAPN)
    // UG MJR KIN ADV.ARE (KIN1)
    // UG MJR LAS CULT PERSP
    // UG MJR LAS HIST PERSP
    // UG MJR LAS REL/PHIL PERSP
    // UG MJR LAS SOC PERSP
    // UG MJR-Asian Studies
    // UG MJR-Environmental Studies
    // UG MJR-International Studies
    // UG MJR-Urban Studies
    // UG MJR/MNR ART MOD/CONTEMP
    // UG MJR/MNR ART NON-WESTERN
    // UG MJR/MNR ART PRE-MODERN W.
    // UG MNR AAS DISTRIBUTION (MAA1)
    // UG MNR AAS ELECTIVE (MAA2)
    // UG MNR ART HIST. ELECT (AHAM)
    // UG MNR CATH ELECT (MCA1)
    // UG MNR EURO ELECT (MEU1)
    // UG MNR FILM ELECT (MFS1)
    // UG MNR FNAR ELECT (MFA1)
    // UG MNR GSS
};

struct Course {
    const char* subject;
    const char* number;
    const char* name;
    const char* units;
    const char* descr;
    const char* prereq;
    const char* restr;
    const char* college;
    // enum Attribute attributes[16];
};

enum InstructionMode {
    InPerson,
    Online,
    Hybrid,
    NULL_INSTRUCTION_MODE,
};

struct Section {
    struct Course* course;
    int section_num;
    int crn;
    char schedule_type[256];
    enum InstructionMode instruction_mode;
    int* days; // eg MW = {1,0,1,0,0,0,0}
    int begin_time;
    int end_time;
    char start_date[256];
    char end_date[256];
    char building[256];
    char room[16];
    int capacity;
    int enrollment;
    char instructor_first[256];
    char instructor_last[256];
    char instructor_email[256];
    char college[256];
};

int art();

/**
 * logs in w/ given username
 */
int login(char* user, mongoc_collection_t* users_collection);

int logout();

/**
 * returns 0 if successful addition, 1 if not
 */
int add(int crn, char* plan, mongoc_collection_t* plans_collection, mongoc_collection_t* sections_collection);

int rm(int crn, char* plan, mongoc_collection_t* plans_collection, mongoc_collection_t* sections_collection);

int rmplan(const char* plan, mongoc_collection_t* plans_collection);

int view(char* plan, mongoc_collection_t* plans_collection);

int viewplans(mongoc_collection_t* collection);

/**
 * displays results of browse - sections
 * returns 1 if error
 */
int browse(char subject[5], char number[16], enum InstructionMode instruction_mode, int n_attrs, enum Attribute attrs[16], char instructor[256], int n_keywords, char** keywords, int verbose, mongoc_collection_t* sections_collection, mongoc_collection_t* courses_collection);

/**
 * applies set plan to main
 * returns 1 if error
 */
int apply(char* plan, mongoc_collection_t* plans_collection);

/**
 * searches catalog (not necessarily what's offered this sem)
 * prints search results - courses
 * returns 1 if error
 */
int cbrowse(char subject[5], char number[16], int n_attrs, enum Attribute attrs[16], int n_keywords, char** keywords, int verbose, mongoc_collection_t* courses_collection);


/**
 * displays weekly schedule
 * returns 1 if error
 */
int schedule(char* plan, mongoc_collection_t* courses_collection, mongoc_collection_t* plans_collection, mongoc_collection_t* sections_collection);

#endif