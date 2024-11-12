#include "helper_client.h"

ClientTokens client_tokens_global[MAX_INPUT_SIZE];
char users_ids_global[MAX_INPUT_SIZE][16];
InputCollection inputCollection;
ClientsInfo clients_info_global[MAX_INPUT_SIZE];
int clients_info_counter = 0;

InputCollection read_input_collection(char path[MAX_INPUT_SIZE])
{
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening clients.in!");
    }

    InputCollection inputCollection;
    inputCollection.input = malloc(sizeof(Input) * MAX_INPUT_SIZE);
    inputCollection.count = 0;

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        // overwrite the '\n'
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }

        char *token = strtok(line, ",");
        if (token != NULL) {
            Input input;

            // get the user_id
            strncpy(input.user_id, token, sizeof(input.user_id) - 1);
            input.user_id[sizeof(input.user_id) - 1] = '\0';
            token = strtok(NULL, ",");

            // get he operation_type
            if (token != NULL) {
                strncpy(input.operation_type, token, sizeof(input.operation_type) - 1);
                input.operation_type[sizeof(input.operation_type) - 1] = '\0';
                token = strtok(NULL, ",");
            }

            // get the argument
            if (token != NULL) {
                strncpy(input.argument, token, sizeof(input.argument) - 1);
                input.argument[sizeof(input.argument) - 1] = '\0';
            }

            inputCollection.input[inputCollection.count] = input;
            inputCollection.count++;
        }
    }

    free(line);
    fclose(file);

    return inputCollection;
}

void init_clients_info_global()
{
    for (int i = 0; i < inputCollection.count; i++)
    {
        clients_info_global[i].access_token = malloc(16);
        clients_info_global[i].refresh_token = malloc(16);
        clients_info_global[i].user_id = malloc(16);

        // initialize the values
        memcpy(clients_info_global[i].access_token, "empty", 6);
        memset(clients_info_global[i].refresh_token, 0, 16);
        memset(clients_info_global[i].user_id, 0, 16);
        clients_info_global[i].availability = -1;
        clients_info_global[i].automatic_refresh = 0;
    }
}

int find_index_by_user_id_for_clients_info(char *user_id)
{
    for (int i = 0; i < inputCollection.count; i++)
    {
        if (strcmp(clients_info_global[i].user_id, user_id) == 0)
        {
            return i;
        }
    }
    return -1;
}
