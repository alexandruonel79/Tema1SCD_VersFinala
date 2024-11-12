#ifndef HELPER_OAUTH_H
#define HELPER_OAUTH_H
#include "Oauth.h"
#include <stdbool.h>
#define MAX_CHAR_SIZE 100

typedef struct{
    char filename[MAX_CHAR_SIZE][MAX_CHAR_SIZE];
    int count;
} Resources;

typedef struct{
    char user_id[MAX_CHAR_SIZE][16];
    int count;
} Users;

typedef struct {
	int read;
	int insert;
	int modify;
	int execute;
	int delete;
} Permissions;

typedef struct {
	char *name;
	Permissions perm;
} File;

// to be used only as the USER to give permissions
// the server takes the permissions from the USER, from the signed file
typedef struct {
    File *files;
    int count;
} Approvals;

typedef struct {
    char auth_token[MAX_CHAR_SIZE][16];
    char refresh_token[MAX_CHAR_SIZE][16];
    char access_token[MAX_CHAR_SIZE][16];
    int availability;
    int automatic_refresh;
    Approvals files_permissions;
} Tokens;

extern int availabilty_global;
extern Resources resources_global;
extern Users users_global;
extern int fifo_counter;
extern Tokens tokens_global[MAX_CHAR_SIZE];
extern char *approvals_path;
extern FILE *approvals_file_ptr;

Resources read_resources(char path[MAX_CHAR_SIZE]);
Users read_users(char path[MAX_CHAR_SIZE]);
char* simulate_user_prompt_file_permissions();
Approvals create_approvals_from_string(char *files_and_perms);
bool is_user_in_db(char *user_id);
int get_user_index(char *user_id);
void set_initial_tokens_empty();
bool user_has_any_permission(Approvals approvals);
int find_user_id_by_access_token(char *access_token);
bool exists_resource(char *filename);
void encrypt(SignedData *data);
void decrypt(SignedData *data);
int get_user_index_by_auth_token(char *auth_token);
void open_approvals_file(char *path);
bool can_user_access_file(Approvals approvals, char *filename, char *op_type);
int find_user_id_by_refresh_token(char *refresh_token);


#endif
