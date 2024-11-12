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

    // Open the file in read mode
    file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Error opening resources.db!");
    }

    Resources resources;

    // Read the first number (n) from the first line
    if (fscanf(file, "%d", &n) != 1)
    {
        printf("Error reading the number of resources from resources.db!\n");
        fclose(file);
    }

    resources.count = n;
    // read the last \n after scanf
    fgets(str, sizeof(str), file);
    // Read the next n strings
    for (int i = 0; i < n; i++)
    {
        if (fgets(str, sizeof(str), file) != NULL)
        {
            // Remove the newline character by replacing it with '\0'
            int len = strlen(str);
            if (len > 0 && str[len - 1] == '\n')
            {
                str[len - 1] = '\0';
            }

            // printf("String %d: %s\n", i + 1, str);
            // save the string in the file
            strncpy(resources.filename[i], str, strlen(str));
        }
        else
        {
            printf("Error reading string %d\n", i + 1);
        }
    }

    // Close the file
    fclose(file);

    return resources;
}

Users read_users(char path[MAX_CHAR_SIZE])
{
    FILE *file;
    int n;
    char str[MAX_CHAR_SIZE];

    // Open the file in read mode
    file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Error opening userIDS.db!");
    }

    Users users;

    // Read the first number (n) from the first line
    if (fscanf(file, "%d", &n) != 1)
    {
        printf("Error reading the number of user_ids from userIDS.db!\n");
        fclose(file);
    }

    users.count = n;
    // read the last \n after scanf
    fgets(str, sizeof(str), file);
    // Read the next n strings
    for (int i = 0; i < n; i++)
    {
        if (fgets(str, sizeof(str), file) != NULL)
        {
            // Remove the newline character by replacing it with '\0'
            int len = strlen(str);
            if (len > 0 && str[len - 1] == '\n')
            {
                str[len - 1] = '\0';
            }

            // printf("String %d: %s\n", i + 1, str);
            // save the string in the file
            strncpy(users.user_id[i], str, strlen(str));
        }
        else
        {
            printf("Error reading string %d\n", i + 1);
        }

        // users.auth_token[i][0] = '\0';
        // users.refresh_token[i][0] = '\0';
        // users.access_token[i][0] = '\0';
    }

    // Close the file
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

    // printf("In create app the string is %s\n", files_and_perms_copy);
    Approvals approvals;
    approvals.files = malloc(sizeof(File) * MAX_CHAR_SIZE);
    int count = 0;
    // use strtok to get all tokens
    char *token = strtok(files_and_perms_copy, ",");
    while (token != NULL)
    {
        File file;
        file.perm.read = 0;
        file.perm.insert = 0;
        file.perm.modify = 0;
        file.perm.execute = 0;
        file.perm.delete = 0;
        // first is the filename
        // printf("Filename: %s\n", token);
        file.name = malloc(strlen(token) + 1);
        strncpy(file.name, token, strlen(token) + 1);
        token = strtok(NULL, ",");
        // second are permissions
        // printf("Permissions: %s\n", token);
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
        // printf("stored\n");
        approvals.files[count] = file;
        count += 1;
    }

    approvals.count = count;

    // Close the file
    // fclose(file);
    // print all approvals
    // for (int i = 0; i < approvals.count; i++)
    // {
    //     printf("OUT = Filename: %s\n", approvals.file.file_val[i].name);
    //     printf("OUT = Permissions: %d %d %d %d %d\n", approvals.file.file_val[i].perm.read, approvals.file.file_val[i].perm.insert, approvals.file.file_val[i].perm.modify, approvals.file.file_val[i].perm.execute, approvals.file.file_val[i].perm.delete);
    // }
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
        // set the tokens to empty '\0'
        memset(tokens_global[i].auth_token, 0, sizeof(tokens_global[i].auth_token));
        memset(tokens_global[i].refresh_token, 0, sizeof(tokens_global[i].refresh_token));
        memset(tokens_global[i].access_token, 0, sizeof(tokens_global[i].access_token));
        // and the permissions to 0
        // tokens_global[i].availability = -1;
        tokens_global[i].automatic_refresh = 0;
        tokens_global[i].files_permissions.count = 0;
        // tokens_global[i].files_permissions.file.file_val = malloc(sizeof(File) * MAX_CHAR_SIZE);
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
        // debug print
        // printf("Comparing %s with %s\n", resources_global.filename[i], filename);
        /// TODO CHESTIE DUBIOASA, NU MI DAU SEAMA MOMENTAN
        if (strncmp(resources_global.filename[i], filename, strlen(resources_global.filename[i])) == 0)
        {
            return true;
        }
    }
    return false;
}

// Simple function to alter auth_token by shifting each character
void encrypt(SignedData *data)
{
    if (data && data->auth_token)
    {
        for (size_t i = 0; i < strlen(data->auth_token); i++)
        {
            data->auth_token[i] += 5;
        }
    }
}

// Function to reverse the alteration by shifting characters back
void decrypt(SignedData *data)
{
    if (data && data->auth_token)
    {
        for (size_t i = 0; i < strlen(data->auth_token); i++)
        {
            data->auth_token[i] -= 5;
        }
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
    // Open the file in read mode
    approvals_file_ptr = fopen(path, "r");
    if (approvals_file_ptr == NULL)
    {
        perror("Error opening approvals.db!");
    }
}

bool can_user_access_file(Approvals approvals, char *filename, char *op_type)
{
    // debug printf
    // printf("Checking if user can access file %s with op_type %s\n", filename, op_type);
    // // printf approvals
    // for (int i = 0; i < approvals.count; i++)
    // {
    //     printf("File %s\n", approvals.files[i].name);
    //     printf("Permissions %d %d %d %d %d\n", approvals.files[i].perm.read, approvals.files[i].perm.insert, approvals.files[i].perm.modify, approvals.files[i].perm.execute, approvals.files[i].perm.delete);
    // }

    for (int i = 0; i < approvals.count; i++)
    {

        // debug strcmp and print strcmp result
        // printf("Comparing %s with %s and strcmp res is %d\n", approvals.files[i].name, filename, strcmp(approvals.files[i].name, filename));
        // printf("Comparing '%s' with '%s'\n", approvals.files[i].name, filename);
        // for (int j = 0; j < strlen(approvals.files[i].name); j++)
        // {
        //     printf("Approvals file[%d] char: '%c' ASCII: %d\n", j, approvals.files[i].name[j], approvals.files[i].name[j]);
        // }
        // for (int j = 0; j < strlen(filename); j++)
        // {
        //     printf("Filename[%d] char: '%c' ASCII: %d\n", j, filename[j], filename[j]);
        // }

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

int find_user_id_by_refresh_token(char *refresh_token){
    for (int i = 0; i < users_global.count; i++)
    {
        if (strcmp((char *)tokens_global[i].refresh_token, refresh_token) == 0)
        {
            return i;
        }
    }
    return -1;
}

char *format_op_output(char *op_type){
    if (strcmp(op_type, "R") == 0)
    {
        return "READ";
    }
    else if (strcmp(op_type, "I") == 0)
    {
        return "INSERT";
    }
    else if (strcmp(op_type, "X") == 0)
    {
        return "EXECUTE";
    }
    else if (strcmp(op_type, "D") == 0)
    {
        return "DELETE";
    }
    else if (strcmp(op_type, "M") == 0)
    {
        return "MODIFY";
    }

    return "ERROR";
}