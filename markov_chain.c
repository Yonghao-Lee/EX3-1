#include "markov_chain.h"
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */

char* my_strdup(const char* s)
{
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup)
    {
        memcpy(dup, s, len);
    }
    return dup;
}

int get_random_number(int max_number)
{
    return rand() % max_number;
}


Node* get_node_from_database(MarkovChain* markov_chain,
    char* data_ptr)
{
    if (markov_chain == NULL || markov_chain->database == NULL)
    {
        return NULL;
    }
    LinkedList* database = markov_chain->database;
    Node* current_node = database->first;
    while (current_node != NULL)
    {
        if (strcmp((current_node->data)->data, data_ptr) == 0)
        {
            return current_node;
        }
        current_node = current_node->next;
    }
    return NULL;
}


Node* add_to_database(MarkovChain* markov_chain, char* data_ptr)
{
    //data pointer here is the string
    if (markov_chain == NULL || markov_chain->database == NULL)
    {
        return NULL;
    }
    LinkedList* database = markov_chain->database;
    Node* current_node = database->first;

    // Check if data_ptr is already in the database
    while (current_node != NULL)
    {
        if (strcmp((current_node->data)->data, data_ptr) == 0)
        {
            return current_node; // Found an existing node
        }
        current_node = current_node->next;
    }

    // Data not found, creat a new node
    Node* new_node = malloc(sizeof(Node));
    if (new_node == NULL)
    {
        printf("Memory allocation failed in "
               "add_to_database()\n");
        return NULL;
    }
    // Allocate memory for the markov node
    new_node->data = malloc(sizeof(MarkovNode));
    if (new_node->data == NULL)
    {
        printf("Memory allocation failed in "
               "add_to_database()\n");
        free(new_node);
        return NULL;
    }

    // Duplicate the string data of the markov node
    new_node->data->data = my_strdup(data_ptr);
    if (new_node->data->data == NULL)
    {
        printf("Memory allocation failed in "
               "add_to_database()\n");
        free(new_node->data);
        free(new_node);
        return NULL;
    }

    new_node->data->frequency_list = NULL;

    new_node->next = NULL;

    if (database->first == NULL)
    {
        database->first = new_node;
        database->last = new_node;
    }
    else
    {
        database->last->next = new_node;
        database->last = new_node;
    }

    return new_node;
}

bool is_terminal_word(const char* data)
{
    if (data == NULL || strlen(data) == 0)
    {
        return false;
    }
    char last_char = data[strlen(data) - 1];

    return (last_char == '.');
}


int add_node_to_frequency_list(MarkovNode* first_node,
    MarkovNode* second_node)
{
    if (first_node == NULL || second_node == NULL
        || first_node->data == NULL || second_node->data == NULL)
    {
        return EXIT_FAILURE;
    }
    if (is_terminal_word(first_node->data))
    {
        return 0;
    }
    MarkovNodeFrequency* frequency_list =
        first_node->frequency_list;
    if (frequency_list == NULL)
    {
        frequency_list = malloc(2 *
            sizeof(MarkovNodeFrequency));
        if (frequency_list == NULL)
        {
            printf("Memory allocation failed "
                   "in add_node_to_frequency_list()\n");
            return EXIT_FAILURE;
        }
        frequency_list[0].markov_node = second_node;
        frequency_list[1].markov_node = NULL;
        frequency_list[0].frequency = 1;
        frequency_list[1].frequency = 0;
        first_node->frequency_list = frequency_list;
        return EXIT_SUCCESS;
    }
    int i = 0;
    for (i = 0; frequency_list[i].markov_node != NULL; i++)
    {
        if (frequency_list[i].markov_node == second_node)
        {
            frequency_list[i].frequency++;
            return EXIT_SUCCESS;
        }
    }

    // Node not found, reallocate memory to include it
    MarkovNodeFrequency* new_list =
        realloc(frequency_list,
        (i + 2)
        *sizeof(MarkovNodeFrequency));
    if (new_list == NULL)
    {
        printf("Memory allocation failed in "
               "add_node_to_frequency_list()\n");
        return EXIT_FAILURE;
    }

    // Update the frequency list
    frequency_list = new_list;
    first_node->frequency_list = frequency_list;
    frequency_list[i].markov_node = second_node;
    frequency_list[i].frequency = 1;
    frequency_list[i + 1].markov_node = NULL;
    frequency_list[i + 1].frequency = 0;
    return EXIT_SUCCESS;
}


void free_database(MarkovChain** ptr_chain)
{
    if (ptr_chain == NULL || *ptr_chain == NULL)
    {
        return; // Nothing to free
    }

    MarkovChain* chain = *ptr_chain;
    LinkedList* database = chain->database;

    // Free all nodes in the linked list
    Node* current_node = database->first;
    while (current_node != NULL)
    {
        Node* next_node = current_node->next;

        // Free the MarkovNode within the current node
        MarkovNode* markov_node = current_node->data;
        if (markov_node != NULL)
        {
            // Free the frequency_list in the MarkovNode
            if (markov_node->frequency_list != NULL) {
                free(markov_node->frequency_list);
                markov_node->frequency_list = NULL;
            }
            if (markov_node->data != NULL) {
                free(markov_node->data);
                markov_node->data = NULL;
            }

            // Free the MarkovNode itself
            free(markov_node);
        }

        // Free the current node
        free(current_node);

        current_node = next_node;
    }

    // Free the linked list structure
    free(database);

    // Free the MarkovChain itself
    free(chain);

    // Set the pointer to NULL to avoid dangling pointers
    *ptr_chain = NULL;
}

int get_chain_length(const LinkedList* database)
{
    if (database == NULL)
    {
        return 0;
    }
    int length = 0;
    Node* current_node = database->first;
    while (current_node != NULL)
    {
        length++;
        current_node = current_node->next;
    }
    return length;
}

MarkovNode* get_first_random_node(MarkovChain* markov_chain)
{
    if (markov_chain == NULL || markov_chain->database == NULL
        || markov_chain->database->first == NULL)
    {
        return NULL;
    }
    const int total_length =
        get_chain_length(markov_chain->database);
    if (total_length == 0)
    {
        return NULL;
    }

    Node* current_node;
    do
    {
        int random_index = get_random_number(total_length);

        current_node = markov_chain->database->first;
        for (int i = 0; i < random_index; i++)
        {
            current_node = current_node->next;
        }
    }
    while (is_terminal_word(current_node->data->data));
    return current_node->data;
}


MarkovNode* get_next_random_node(MarkovNode* cur_markov_node)
{
    if (cur_markov_node == NULL ||
        cur_markov_node->frequency_list == NULL)
    {
        return NULL;
    }

    int total_frequency = 0;
    MarkovNodeFrequency* frequency_list =
        cur_markov_node->frequency_list;

    for (int i = 0; frequency_list[i].markov_node != NULL; i++)
    {
        total_frequency += frequency_list[i].frequency;
    }

    int random_num = get_random_number(total_frequency);
    int frequency = 0;
    for (int i = 0; frequency_list[i].markov_node != NULL; i++)
    {
        frequency += frequency_list[i].frequency;
        if (frequency > random_num)
        {
            return frequency_list[i].markov_node;
        }
    }
    return NULL;
}

void generate_tweet(MarkovNode* first_node, int max_length)
{
    if (first_node == NULL ||
        first_node->frequency_list == NULL)
    {
        return;
    }
    MarkovNode* current_node = first_node;
    int word_count = 0;

    while (current_node != NULL && word_count < max_length)
    {
        printf("%s", current_node->data);
        if (is_terminal_word(current_node->data))
        {
            break;
        }
        printf(" ");

        current_node = get_next_random_node(current_node);
        if (current_node == NULL)
        {
            break;
        }
        word_count++;
    }
    printf("\n");
}
