/**
 * handle commands from main
 */

#ifndef HANDLE
#define HANDLE

#include <mongoc.h>

int handle_art();
int handle_login(int argc, char** argv, mongoc_client_t* client);
int handle_logout();
int handle_add(int argc, char** argv, mongoc_client_t* client);
int handle_rm(int argc, char** argv, mongoc_client_t* client);
int handle_rmplan(int argc, char** argv, mongoc_client_t* client);
int handle_view(int argc, char** argv, mongoc_client_t* client);
int handle_viewplans(mongoc_client_t* client);
int handle_browse(int argc, char** argv, mongoc_client_t* client);
int handle_apply(int argc, char** argv, mongoc_client_t* client);
int handle_cbrowse(int argc, char** argv, mongoc_client_t* client);
int handle_schedule(int argc, char** argv, mongoc_client_t* client);

#endif