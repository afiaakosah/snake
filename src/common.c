#include "common.h"

#include <stdlib.h>

// TODO: define global variables needed for your snake! (part 1A)

// Definition of global variables for game status.
int g_game_over;
int g_score;

//Define global variables for user's name
int g_name_len;
char* g_name;

//Definition of global variables for the snake
// int g_snake_position; 
// enum input_key g_snake_direction;

/** Sets the seed for random number generation.
 * Arguments:
 *  - `seed`: the seed.
 */
void set_seed(unsigned seed) {
    /* DO NOT MODIFY THIS FUNCTION */
    srand(seed);
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Returns a random index in [0, size)
 * Arguments:
 *  - `size`: the upper bound for the generated value (exclusive).
 */
unsigned generate_index(unsigned size) {
    /* DO NOT MODIFY THIS FUNCTION */
    return rand() % size;
    /* DO NOT MODIFY THIS FUNCTION */
}