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
        exit(EXIT_FAILURE);
    }

    InputCollection inputCollection;
    inputCollection.input = malloc(sizeof(Input) * MAX_INPUT_SIZE);
    inputCollection.count = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // read line by line
    while ((read = getline(&line, &len, file)) != -1) {
        // Remove newline character at the end if it exists
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        // Tokenize the line by commas
        char *token = strtok(line, ",");
        if (token != NULL) {
            Input input;

            // First token is the user_id
            strncpy(input.user_id, token, sizeof(input.user_id) - 1);
            input.user_id[sizeof(input.user_id) - 1] = '\0';
            token = strtok(NULL, ",");

            // Second token is the operation_type
            if (token != NULL) {
                strncpy(input.operation_type, token, sizeof(input.operation_type) - 1);
                input.operation_type[sizeof(input.operation_type) - 1] = '\0';
                token = strtok(NULL, ",");
            }

            // Third token is the argument
            if (token != NULL) {
                strncpy(input.argument, token, sizeof(input.argument) - 1);
                input.argument[sizeof(input.argument) - 1] = '\0';
            }

            // Save the input in the collection
            inputCollection.input[inputCollection.count] = input;
            inputCollection.count++;
        }
    }

    // Clean up and close file
    free(line);
    fclose(file);

    return inputCollection;
}

void init_clients_info_global()
{
    for (int i = 0; i < inputCollection.count; i++)
    {
        // allocate memory for the clients info
        clients_info_global[i].access_token = malloc(16);
        clients_info_global[i].refresh_token = malloc(16);
        clients_info_global[i].user_id = malloc(16);

        // set the values to empty
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
