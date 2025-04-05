/**
 * Commands for creg cli
 */

#ifndef COMMANDS
#define COMMANDS

/**
 * Course level
 */
enum Level {
    Certficate,
    Continuing,
    Credential,
    Doctoral,
    Graduate,
    GraduateLaw,
    GraduateLLM,
    Law,
    NonCredit,
    Professional,
    Undergraduate,
};

char* levelStr(enum Level l);

/**
 * course attributes
 */
enum Attribute {
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

char* attrStr(enum Attribute a);

struct Course {
    char subject[4];
    int number;
    char name[256];
    enum Level level;
    enum Attribute attributes[16];
};

enum InstructionMode {
    Hybrid,
    InPerson,
    NonTraditional,
    OnlineAsynch,
    OnlineSynch,
    Traditional,
};

char* instrModeStr(enum InstructionMode i);

struct Section {
    struct Course* course;
    int sectionNum;
    char scheduleType[256];
    char campus[256];
    enum InstructionMode instructionMode;
    char meetingType [256];
    int days[7]; // eg MW = {1,0,1,0,0,0,0}
    int beginTime;
    int endTime;
    char startDate[256];
    char endDate[256];
    char building[256];
    int room;
    int capacity;
    int enrollment;
    char instructorFirst[256];
    char instructorLast[256];
    char instructorEmail[256];
    char college[256];
};

/**
 * returns 0 if successful login, 1 if not
 */
int login(char* user);

/**
 * returns 0 if successful addition, 1 if not
 */
int add(int crn);

/**
 * displays results of browse
 */
void browse(int subject[4], int number, enum InstructionMode instructionMode, char campus[256], char level[256], char attrs[256], char instructor[256]);

/**
 * set working plan
 * blank input indicates main
 */
void plan(char* name);

/**
 * applies set plan to main
 */
void apply(char* planName);

/**
 * searches catalog (not necessarily what's offered this sem)
 * prints search results
 */
void cbrowse();

/**
 * displays degree progress
 */
void degree();

/**
 * displays weekly schedule
 */
void schedule(int format);

#endif