/**
 * username methods
 */

#ifndef USER_FILE
#define USER_FILE ".creg_user"

char *get_user_file_path();
char *load_username();
void save_username(const char *username);

#endif