cmake_minimum_required(VERSION 3.10)

# Project name and language
project(MarkovChainTweetGenerator C)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Define the source files
set(SOURCES
        tweets_generator.c              # Main program
        markov_chain.c      # Markov Chain implementation
        linked_list.c       # Linked List implementation (if used in your project)
)

# Define the header files
set(HEADERS
        markov_chain.h      # Markov Chain header
        linked_list.h       # Linked List header (if applicable)
)

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries (if needed, e.g., math library)
target_link_libraries(${PROJECT_NAME} m)