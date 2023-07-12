#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linked_list.h"
#include "mbstrings.h"

int get_next_position(enum input_key input, int initial_pos, size_t width, enum input_key snake_dir){

    int next_position; 

    switch (input) {
        case (INPUT_LEFT):
            next_position = initial_pos - 1;
            break;
        case (INPUT_RIGHT):
            next_position = initial_pos + 1;
            break;
        case (INPUT_UP):
            next_position = initial_pos - width;
            break;
        case (INPUT_DOWN):
            next_position = initial_pos + width;
            break;
        case (INPUT_NONE):
            // if there is no input, move the snake in the 
            //same direction it is already moving in
            switch (snake_dir) {
            case (INPUT_LEFT):
                next_position = initial_pos - 1;
            break;
            case (INPUT_RIGHT):
                next_position = initial_pos + 1;
                break;
            case (INPUT_UP):
                next_position = initial_pos - width;
                break;
            case (INPUT_DOWN):
                next_position = initial_pos + width; 
                break;
            case (INPUT_NONE):
            break;

        }
    }

    return next_position;

}

/** Updates the game by a single step, and modifies the game information
 * accordingly. Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: width of the board.
 *  - height: height of the board.
 *  - snake_p: pointer to your snake struct (not used until part 2!)
 *  - input: the next input.
 *  - growing: 0 if the snake does not grow on eating, 1 if it does.
 */
void update(int* cells, size_t width, size_t height, snake_t* snake_p,
            enum input_key input, int growing) {
    // `update` should update the board, your snake's data, and global
    // variables representing game information to reflect new state. If in the
    // updated position, the snake runs into a wall or itself, it will not move
    // and global variable g_game_over will be 1. Otherwise, it will be moved
    // to the new position. If the snake eats food, the game score (`g_score`)
    // increases by 1. This function assumes that the board is surrounded by
    // walls, so it does not handle the case where a snake runs off the board.

    // the next position of the snake
    int snake_pos = snake_p->snake_position->data;
    int next_position;
    enum input_key prev_direction = snake_p->snake_direction;
    int is_backing_up = 0;

    //if there is user input, change the snake's direction
    if (input != INPUT_NONE) {
        snake_p->snake_direction = input;
    } 

    // if game is not over, we calculate the new head position
    if (g_game_over == 0) {

        //calculate the next position, given the input & current direction
        next_position = get_next_position(input, snake_pos, width, prev_direction);

        // if snake hits wall, update game to be over
        if (cells[next_position] == FLAG_WALL) {

            g_game_over = 1;

        } else if (cells[next_position] == FLAG_FOOD) { // if snake hits food

            g_score += 1;
            cells[next_position] = FLAG_SNAKE;

            if(growing){
                insert_first(&(snake_p->snake_position), next_position, sizeof(next_position)); 
            } else {
                cells[snake_pos] = FLAG_PLAIN_CELL;
                snake_p->snake_position->data = next_position;
            }
            place_food(cells, width, height);

        } else {

            if (length_list(snake_p->snake_position) > 1){ // the snake is longer than 1 cell
                
                int curr_snake_cell = snake_pos;
                node_t* second_snake_cell = snake_p->snake_position->next;
                int count = 0;

                while (second_snake_cell != NULL) {

                    if (curr_snake_cell == next_position || second_snake_cell->data == next_position) {
                        
                        if(count >= 2) { //if snake is longer than 2 and going into itself, game ends
                            if(next_position != get_last(snake_p->snake_position)){
                               g_game_over = 1;
                               break; 
                            }
                        }

                        is_backing_up = 1;
                        break;
                    }
                    curr_snake_cell = second_snake_cell->data;                    
                    second_snake_cell = second_snake_cell->next;
                    count++;
                }
                
                
                if (is_backing_up) {
                    
                    if(next_position == get_last(snake_p->snake_position) && count >= 2){
                        //if the snake is moving into where it's tail was
                        next_position = get_next_position(input, snake_pos, width, prev_direction);
                    } else {
                        //if the snake is backing up on itself, movement is invalid
                        //snake should carry on in the same direction  
                        next_position = get_next_position(prev_direction, snake_pos, width, prev_direction);  
                    }
                    
                } 

            }

            if (g_game_over == 0) {
                cells[next_position] = FLAG_SNAKE;
                int prev_pos = remove_last(&(snake_p->snake_position));
                cells[prev_pos] = FLAG_PLAIN_CELL;
                insert_first(&(snake_p->snake_position), next_position, sizeof(next_position));
            }
            
        }
    }

} 

/** Sets a random space on the given board to food.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: the width of the board
 *  - height: the height of the board
 */
void place_food(int* cells, size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    unsigned food_index = generate_index(width * height);
    if (*(cells + food_index) == FLAG_PLAIN_CELL) {
        *(cells + food_index) = FLAG_FOOD;
    } else {
        place_food(cells, width, height);
    }
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Prompts the user for their name and saves it in the given buffer.
 * Arguments:
 *  - `write_into`: a pointer to the buffer to be written into.
 */
void read_name(char* write_into) {
    ssize_t length = 0;

    printf("Name > ");
    fflush(stdout); 
    length = read(STDIN_FILENO, write_into, 1000);

    if(length <= 1){
        printf("Name Invalid: must be longer than 0 characters.");
    }

    write_into[length -1] = '\0';

}

/** Cleans up on game over — should free any allocated memory so that the
 * LeakSanitizer doesn't complain.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - snake_p: a pointer to your snake struct. (not needed until part 2)
 */
void teardown(int* cells, snake_t* snake_p) {
    // free(snake_p->snake_position);

    int len = length_list(snake_p->snake_position);

    if (len > 1) {
        node_t* curr = snake_p->snake_position->next;
        node_t* next_node;

        while (len > 1) {
            next_node = curr->next;
            free(curr);
            curr = next_node;
            len--;
        }
    } 

    free(snake_p->snake_position);
    
}