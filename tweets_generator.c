//Don't change the macros!
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"
#define BASE_10 10
#define READ_ALL 42
#define LINE_MAX 1001

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "markov_chain.h"

bool error_parsing_msg(const char* endptr);
int count_words_in_file(const char *file_path);
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain);
MarkovChain* initialize_markov_chain();

int main(int argc, char** argv)
{
  if (argc < 4 || argc > 5)
  {
    printf("%s\n", NUM_ARGS_ERROR);
    return EXIT_FAILURE;
  }
  char* endptr;
  errno = 0;
  unsigned int seed = (int)strtol(argv[1], &endptr, BASE_10);
  srand(seed);

  if (!error_parsing_msg(endptr))
  {
    return EXIT_FAILURE;
  }

  errno = 0;
  int num_tweets = (int)strtol(argv[2], &endptr, BASE_10);
  if (!error_parsing_msg(endptr))
  {
    return EXIT_FAILURE;
  }

  const char* file_path = argv[3];
  const int total_words_in_file = count_words_in_file(file_path);

  int max_words_to_read = READ_ALL;
  if (argc == 5)
  {
    errno = 0;
    max_words_to_read = (int)strtol(argv[4], &endptr, BASE_10);
    if (!error_parsing_msg(endptr) || max_words_to_read <= 0)
    {
      return EXIT_FAILURE;
    }
    if (max_words_to_read > total_words_in_file){
      max_words_to_read = total_words_in_file;
    }
  }
  // printf("the seed in decimal form :%d\n", seed);
  // printf("Number of tweets to generate:%d\n ", num_tweets

  // Initialize the markov chain
    MarkovChain *markov_chain = initialize_markov_chain();  if (!markov_chain){
  }

  FILE *file = fopen(file_path, "r");
  if (!file){
    printf("Unable to open file.\n");
    free_database(&markov_chain);
    return EXIT_FAILURE;
  }

  if (fill_database(file, max_words_to_read, markov_chain) != EXIT_SUCCESS) {
    printf("Error: Failed to populate database.\n");
    fclose(file);
    free_database(&markov_chain);
    return EXIT_FAILURE;
  }


  int tweets_generated = 0; // Track successfully generated tweets

  while (tweets_generated < num_tweets) {
    MarkovNode* first_node = get_first_random_node(markov_chain);
    if (!first_node) {
      printf("Unable to get first node. Retrying...\n");
      continue; // Retry this iteration
    }

    printf("Tweet %d: ", tweets_generated + 1);
    generate_tweet(first_node, 20);
    tweets_generated++; // Increment only on successful generation
  }

  fclose(file);
  free_database(&markov_chain);
  return EXIT_SUCCESS;
}

bool error_parsing_msg(const char* endptr)
{
  if (errno == ERANGE)
  {
    printf("Error: Value out of range.\n");
    return false;
  }
  else if (*endptr != '\0')
  {
    printf("Error: Invalid character '%c' found in input.\n", *endptr);
    return false;
  }
  return true;
}

int count_words_in_file(const char *file_path){
  FILE *file = fopen(file_path, "r");
  if (!file){
    printf("%s\n", FILE_PATH_ERROR);
    return EXIT_FAILURE;
  }
  int word_count = 0;
  char line[LINE_MAX];

  while(fgets(line, LINE_MAX, file)){
    char* token = strtok(line, DELIMITERS);
    while(token){
      word_count++;
      token = strtok(NULL, DELIMITERS);
    }
  }
  fclose(file);
  return word_count;
}

int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
  if (fp == NULL || markov_chain == NULL) {
    return EXIT_FAILURE;
  }

  char line[LINE_MAX];
  Node *prev = NULL;   // Track the previous node
  int words_processed = 0;

  // Read each line from the file
  while (fgets(line, LINE_MAX, fp)) {
    // Tokenize the line into words
    char *token = strtok(line, DELIMITERS);
    while (token != NULL && (words_to_read == READ_ALL ||
      words_processed < words_to_read)) {
      Node *current_node = add_to_database(markov_chain, token);
      if (current_node == NULL) {
        return EXIT_FAILURE; // Handle memory allocation failure
      }

      if (prev != NULL) {
        if (add_node_to_frequency_list(prev->data,
          current_node->data) != EXIT_SUCCESS) {
          return EXIT_FAILURE; // Handle failure to link nodes
        }
      }

      // Update `prev` and process the next token
      prev = current_node;
      token = strtok(NULL, DELIMITERS);
      words_processed++;
    }
    prev = NULL;
    // Stop processing if the required number of words is reached
    if (words_to_read != READ_ALL && words_processed >= words_to_read) {
      break;
    }
  }

  return EXIT_SUCCESS; // Successfully processed the words
}

MarkovChain* initialize_markov_chain() {
  // Allocate memory for the MarkovChain
  MarkovChain* markov_chain = malloc(sizeof(MarkovChain));
  if (markov_chain == NULL) {
    printf("Error: Memory allocation for MarkovChain failed.\n");
    return NULL;
  }

  // Allocate and initialize the linked list (database)
  markov_chain->database = malloc(sizeof(LinkedList));
  if (markov_chain->database == NULL) {
    printf("Error: Memory allocation for database failed.\n");
    free(markov_chain); // Free previously allocated memory
    return NULL;
  }
  markov_chain->database->first = NULL;
  markov_chain->database->last = NULL;

  return markov_chain;
}
