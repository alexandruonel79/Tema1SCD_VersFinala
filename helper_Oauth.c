#include "helper_Oauth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int availabilty_global = -1;
Resources resources_global;
Users users_global;
File files_global;
// to be used only as the USER to give permissions
// the server takes the permissions from the USER, from the signed file
Approvals approvals_global;
int fifo_counter = 0;
Tokens tokens_global[MAX_CHAR_SIZE];
char *approvals_path;
FILE *approvals_file_ptr;

Resources read_resources(char path[MAX_CHAR_SIZE])
{
    FILE *file;
    int n;
    char str[MAX_CHAR_SIZE];

    file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Error opening resources.db!");
    }

    Resources resources;
    fscanf(file, "%d", &n);
    resources.count = n;
    // read the last \n after scanf
    fgets(str, sizeof(str), file);
    // read the 'n' next strings
    for (int i = 0; i < resources.count; i++)
    {
        fgets(str, sizeof(str), file);

        // overwrite the '\n'
        if (str[strlen(str) - 1] == '\n')
        {
            str[strlen(str) - 1] = '\0';
        }

        strncpy(resources.filename[i], str, strlen(str));
    }

    fclose(file);
    return resources;
}

Users read_users(char path[MAX_CHAR_SIZE])
{
    FILE *file;
    int n;
    char str[MAX_CHAR_SIZE];

    file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Error opening userIDS.db!");
    }

    Users users;

    fscanf(file, "%d", &n);
    users.count = n;
    // read the last \n after scanf
    fgets(str, sizeof(str), file);
    // read the 'n' next strings
    for (int i = 0; i < users.count; i++)
    {
        fgets(str, sizeof(str), file);

        // overwrite the '\n'
        if (str[strlen(str) - 1] == '\n')
        {
            str[strlen(str) - 1] = '\0';
        }

        strncpy(users.user_id[i], str, strlen(str));
    }

    fclose(file);

    return users;
}
char *simulate_user_prompt_file_permissions()
{
    // read and return the first line from the approvals file
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    read = getline(&line, &len, approvals_file_ptr);
    if (read == -1)
    {
        perror("Error reading from approvals.db!");
    }
    return line;
}
Approvals create_approvals_from_string(char *files_and_perms)
{
    // create a copy of the string
    char *files_and_perms_copy = malloc(strlen(files_and_perms) + 1);
    strncpy(files_and_perms_copy, files_and_perms, strlen(files_and_perms) + 1);

    Approvals approvals;
    approvals.files = malloc(sizeof(File) * MAX_CHAR_SIZE);
    int count = 0;
    // separate using strtok
    char *token = strtok(files_and_perms_copy, ",");
    while (token != NULL)
    {
        File file;
        file.perm.read = 0;
        file.perm.insert = 0;
        file.perm.modify = 0;
        file.perm.execute = 0;
        file.perm.delete = 0;
        // first is the name
        file.name = malloc(strlen(token) + 1);
        strncpy(file.name, token, strlen(token) + 1);
        token = strtok(NULL, ",");
        // second are permissions
        // create a copy of the token
        char *perm = malloc(strlen(token) + 1);
        strncpy(perm, token, strlen(token) + 1);
        // check each character and set the permissions
        for (int i = 0; i < strlen(perm); i++)
        {
            if (perm[i] == 'R')
            {
                file.perm.read = 1;
            }
            else if (perm[i] == 'I')
            {
                file.perm.insert = 1;
            }
            else if (perm[i] == 'M')
            {
                file.perm.modify = 1;
            }
            else if (perm[i] == 'X')
            {
                file.perm.execute = 1;
            }
            else if (perm[i] == 'D')
            {
                file.perm.delete = 1;
            }
        }
        token = strtok(NULL, ",");
        // save the file in the approvals
        approvals.files[count] = file;
        count += 1;
    }

    approvals.count = count;

    return approvals;
}

bool is_user_in_db(char *user_id)
{
    for (int i = 0; i < users_global.count; i++)
    {
        if (strcmp(users_global.user_id[i], user_id) == 0)
        {
            return true;
        }
    }
    return false;
}

int get_user_index(char *user_id)
{
    for (int i = 0; i < users_global.count; i++)
    {
        if (strcmp(users_global.user_id[i], user_id) == 0)
        {
            return i;
        }
    }
    return -1;
}

void set_initial_tokens_empty()
{
    for (int i = 0; i < users_global.count; i++)
    {
        memset(tokens_global[i].auth_token, 0, sizeof(tokens_global[i].auth_token));
        memset(tokens_global[i].refresh_token, 0, sizeof(tokens_global[i].refresh_token));
        memset(tokens_global[i].access_token, 0, sizeof(tokens_global[i].access_token));
        tokens_global[i].availability = -1;
        tokens_global[i].automatic_refresh = 0;
        tokens_global[i].files_permissions.count = 0;
    }
}

bool user_has_any_permission(Approvals approvals)
{
    for (int i = 0; i < approvals.count; i++)
    {
        if (approvals.files[i].perm.read == 1 || approvals.files[i].perm.insert == 1 || approvals.files[i].perm.modify == 1 || approvals.files[i].perm.execute == 1 || approvals.files[i].perm.delete == 1)
        {
            return true;
        }
    }
    return false;
}

int find_user_id_by_access_token(char *access_token)
{
    for (int i = 0; i < users_global.count; i++)
    {
        if (strcmp((char *)tokens_global[i].access_token, access_token) == 0)
        {
            return i;
        }
    }
    return -1;
}

bool exists_resource(char *filename)
{
    for (int i = 0; i < resources_global.count; i++)
    {
        if (strncmp(resources_global.filename[i], filename, strlen(resources_global.filename[i])) == 0)
        {
            return true;
        }
    }
    return false;
}

void encrypt(SignedData *data)
{
    for (int i = 0; i < strlen(data->auth_token); i++)
    {
        data->auth_token[i] += 1;
    }
}

void decrypt(SignedData *data)
{
    for (int i = 0; i < strlen(data->auth_token); i++)
    {
        data->auth_token[i] -= 1;
    }
}

int get_user_index_by_auth_token(char *auth_token)
{
    for (int i = 0; i < users_global.count; i++)
    {
        if (strcmp((char *)tokens_global[i].auth_token, auth_token) == 0)
        {
            return i;
        }
    }
    return -1;
}

void open_approvals_file(char *path)
{
    approvals_file_ptr = fopen(path, "r");
    if (approvals_file_ptr == NULL)
    {
        perror("Error opening approvals.db!");
    }
}

bool can_user_access_file(Approvals approvals, char *filename, char *op_type)
{
    for (int i = 0; i < approvals.count; i++)
    {

        if (strcmp(approvals.files[i].name, filename) == 0)
        {

            if (strcmp(op_type, "READ") == 0 && approvals.files[i].perm.read == 1)
            {
                return true;
            }
            else if (strcmp(op_type, "INSERT") == 0 && approvals.files[i].perm.insert == 1)
            {
                return true;
            }
            else if (strcmp(op_type, "EXECUTE") == 0 && approvals.files[i].perm.execute == 1)
            {
                return true;
            }
            else if (strcmp(op_type, "DELETE") == 0 && approvals.files[i].perm.delete == 1)
            {
                return true;
            }
            else if (strcmp(op_type, "MODIFY") == 0 && approvals.files[i].perm.modify == 1)
            {
                return true;
            }
        }
    }

    return false;
}

int find_user_id_by_refresh_token(char *refresh_token)
{
    for (int i = 0; i < users_global.count; i++)
    {
        if (strcmp((char *)tokens_global[i].refresh_token, refresh_token) == 0)
        {
            return i;
        }
    }
    return -1;
}