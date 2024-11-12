#ifndef HELPER_CLIENT_H
#define HELPER_CLIENT_H
#include "Oauth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 100

typedef struct {
    char user_id[16];
    char operation_type[16];
    char argument[MAX_INPUT_SIZE];
} Input;

typedef struct {
    char auth_token[MAX_INPUT_SIZE][16];
    char refresh_token[MAX_INPUT_SIZE][16];
    char access_token[MAX_INPUT_SIZE][16];
} ClientTokens;

typedef struct {
    Input *input;
    int count;
} InputCollection;

typedef struct {
    char *access_token;
    char *refresh_token;
    char *user_id;
    int automatic_refresh;
    int availability;
} ClientsInfo;

extern char users_ids_global[MAX_INPUT_SIZE][16] ;
extern ClientTokens client_tokens_global[MAX_INPUT_SIZE];
extern InputCollection inputCollection;
extern ClientsInfo clients_info_global[MAX_INPUT_SIZE];
extern int clients_info_counter;

InputCollection read_input_collection(char path[MAX_INPUT_SIZE]);
void init_clients_info_global();
int find_index_by_user_id_for_clients_info(char *user_id);


#endif
