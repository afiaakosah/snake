#include "game_setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Some handy dandy macros for decompression
#define E_CAP_HEX 0x45
#define E_LOW_HEX 0x65
#define S_CAP_HEX 0x53
#define S_LOW_HEX 0x73
#define W_CAP_HEX 0x57
#define W_LOW_HEX 0x77
#define DIGIT_START 0x30
#define DIGIT_END 0x39

/** Initializes the board with walls around the edge of the board.
 *
 * Modifies values pointed to by cells_p, width_p, and height_p and initializes
 * cells array to reflect this default board.
 *
 * Returns INIT_SUCCESS to indicate that it was successful.
 *
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 */
enum board_init_status initialize_default_board(int** cells_p, size_t* width_p,
                                                size_t* height_p) {
    *width_p = 20;
    *height_p = 10;
    int* cells = malloc(20 * 10 * sizeof(int));
    *cells_p = cells;

    for (int i = 0; i < 20 * 10; i++) {
        cells[i] = FLAG_PLAIN_CELL;
    }

    // Set edge cells!
    // Top and bottom edges:
    for (int i = 0; i < 20; ++i) {
        cells[i] = FLAG_WALL;
        cells[i + (20 * (10 - 1))] = FLAG_WALL;
    }
    // Left and right edges:
    for (int i = 0; i < 10; ++i) {
        cells[i * 20] = FLAG_WALL;
        cells[i * 20 + 20 - 1] = FLAG_WALL;
    }

    // Add snake
    cells[20 * 2 + 2] = FLAG_SNAKE;

    return INIT_SUCCESS;
}

/** Initialize variables relevant to the game board.
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 *  - snake_p: a pointer to your snake struct (not used until part 2!)
 *  - board_rep: a string representing the initial board. May be NULL for
 * default board.
 */
enum board_init_status initialize_game(int** cells_p, size_t* width_p,
                                       size_t* height_p, snake_t* snake_p,
                                       char* board_rep) {
    enum board_init_status result; 

    //initializes all necessary board data
    g_game_over = 0;
    g_score = 0;
    snake_p->snake_position = NULL;

    //initialise board, using the custom configuration, if not initialise
    //the defualt board
    if(board_rep != NULL){
        result = decompress_board_str(cells_p, width_p, height_p, snake_p, board_rep);
    } else {
        (*snake_p).snake_direction = INPUT_RIGHT;
        int init_snake_pos = 42;
        insert_first(&(snake_p->snake_position), init_snake_pos, sizeof(init_snake_pos));
        result = initialize_default_board(cells_p, width_p, height_p);
    }

    if(result == INIT_SUCCESS){
        place_food(*cells_p, *width_p, *height_p);    
    }

    return result;
}

/**
 * A Helper function to find the first letter 
 * in the passed in string. Arguments:
 * 
 * - str: a pointer to the first char in the string 
 *        in which we will look for a letter. 
*/
char* first_letter(char* start){
    char* ch;
    for (ch = start; *ch >= '0' && *ch <= '9'; ch++) {}
    return ch;
}


/**
 * A Helper function to find the first complete number 
 * in the passed in string. Arguments:
 * 
 * - start: a pointer to the first char in the string 
 *          where the number starts. 
 * - end: a pointer to the char in the string where 
 *        the number ends. 
*/
int get_integer(char* start, char* end) {
    char temp_array[10];
    memset(temp_array, 0, 10);
    strncpy(temp_array, start, end - start + 1);
    return atoi(temp_array);
}


/** Takes in a string `compressed` and initializes values pointed to by
 * cells_p, width_p, and height_p accordingly. Arguments:
 *      - cells_p: a pointer to the pointer representing the cells array
 *                 that we would like to initialize.
 *      - width_p: a pointer to the width variable we'd like to initialize.
 *      - height_p: a pointer to the height variable we'd like to initialize.
 *      - snake_p: a pointer to your snake struct (not used until part 2!)
 *      - compressed: a string that contains the representation of the board.
 * Note: We assume that the string will be of the following form:
 * B24x80|E5W2E73|E5W2S1E72... To read it, we scan the string row-by-row
 * (delineated by the `|` character), and read out a letter (E, S or W) a number
 * of times dictated by the number that follows the letter.
 */
enum board_init_status decompress_board_str(int** cells_p, size_t* width_p,
                                            size_t* height_p, snake_t* snake_p,
                                            char* compressed) {

    // a pointer to the first occurrence of the delimter substring
    char* delimiter_p = strstr(compressed, "|"); 

    // the char between the board dimensions
    char* x_p = strstr(compressed, "x");

    //get the first number after B, the hieght
    *height_p = get_integer(compressed + 1, x_p - 1); 
    //second number, the one before the delimiter, the width
    *width_p = get_integer(x_p + 1, delimiter_p - 1);
    
    // count occurences of "|", verify dimensions (INIT_ERR_INCORRECT_DIMENSIONS)
    int delimiter_count;
    delimiter_count = 0;
    for (int i = 0; i < (int) strlen(compressed); i++) {
        if (compressed[i] == '|') {
            delimiter_count += 1;
        }
    }
    // the delimiter count should equal the number of rows (i.e. the height)
    if (delimiter_count != (int) *height_p) {
        return INIT_ERR_INCORRECT_DIMENSIONS;
    }

    // allocate memory for the cells
    *cells_p = (int*) malloc((*width_p) * (*height_p) * sizeof(int));
    //the first part of compressed
    char* token = strtok(compressed, "|");
    // pass in null so that it continues past the board dimensions
    token = strtok(NULL, "|");

    // initialize board data
    int number_of_snakes = 0;
    int total_cells = 0;
    int* cell_ptr = *cells_p;

    snake_p->snake_direction = INPUT_RIGHT;
    // snake_p->snake_position = 42;

    while (token != NULL) {
        char* token_p = token;
        while (*token_p != '\0') {
            // cell type, W, S, E
            char cell_type = *token_p;

            // tracks # of consecutive cells of same type
            int number_of_cells = 0;

            //get the the first letter in the string
            char* nondigit = first_letter(token_p + 1);
            number_of_cells = get_integer(token_p + 1, nondigit - 1);
            total_cells += number_of_cells;

            if (total_cells > (int) *width_p * (int) *height_p) { 
                free(*cells_p);
                return INIT_ERR_INCORRECT_DIMENSIONS;
            }

            int flag = 0;
            if (cell_type == 'W'){
                flag = FLAG_WALL;
            } else if (cell_type == 'E') {
                flag = FLAG_PLAIN_CELL;
            } else if (cell_type == 'S') {
                flag = FLAG_SNAKE;
                int init_snake_pos = total_cells -1;
                insert_first(&(snake_p->snake_position), init_snake_pos, sizeof(init_snake_pos));
                number_of_snakes += number_of_cells;
                // if the number of snakes is not 1, we throw error
                if (number_of_snakes != 1) {
                    free(*cells_p);
                    return INIT_ERR_WRONG_SNAKE_NUM;
                }
            } else {
                free(*cells_p);
                return INIT_ERR_BAD_CHAR;
            }
            // loop through compressed string and occupy board w/ letters
            for (int i = 0; i < number_of_cells; i++) {
                *cell_ptr = flag;
                cell_ptr += 1;
            }

            token_p = nondigit;
        }

        token = strtok(NULL, "|\\n");
    }

    if (total_cells != (int) *width_p * (int) *height_p) {
        free(*cells_p);
        return INIT_ERR_INCORRECT_DIMENSIONS;
    }
    if (number_of_snakes != 1) {
        free(*cells_p);
        return INIT_ERR_WRONG_SNAKE_NUM;
    }

    return INIT_SUCCESS;
}