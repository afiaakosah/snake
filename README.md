Snake
===========================

## Design Overview:
This is the snake game. 

The board is represented by an array of cells. The `cells` array is defined as an array of integers. This means that each cell on the board has an integer representing what type of cell it is (i.e., a wall, an empty cell, a food cell, or a snake cell). The cells can have *multiple properties*, so the program uses bit flags within an integer value to represent the states. Evne though a 4-byte (32-bit) integer can hold up to 32 bit flags, we only need four for the basic game, defined (in `common.h`). 

The snake always initially moves to the right, as is convention in most snake games. After that, the core functionality is executed in the update function. This function, in game.c, updates the cells array and other game data according to input, including the g_game_over and g_score global variables when needed, and the snake’s position if the game is not over.

For this version of Snake, the core game loop is as follows:
1. wait (for some specified amount of time) for user input;
2. retrieve user input;
3. update the game state based on user input; and
4. render the new game state to the screen.

This snake game supports user-created levels whose board dimensions may differ. To do this, the user will need to encode the state of each square of the board (wall, empty, contains snake, etc.) as a letter. For example, “E” denotes an empty square, “W” denotes a square that contains a wall, etc. To do this, the user must use Run Length Encoding, and input this code as the 3rd argument when running the code. The following example represents a 7-row, 10-column board with some internal walls and a snake at the bottom center: B7x10|W10|W1E4W5|W2E7W1|W1E8W1|W1E4W1E3W1|W1E2S1E1W1E3W1|W10. This encoding is decoded in decompress_board_str(), in game_setup.c where the board is built or a default board if no compression is inputted. 

The game also uses a struct to represent the snake data, allowing us to pack our snake data together in a much more coherent and readable way. It also allows us to handle multiple snake coordinates, enabling the snake to grow. (See this definition in in common.h) The struct includes a field with a pointer to a linked list of coordinates (snake_position), we use this to extend/trim the list as the snake moves (in update()).

The rules of the Snake game are as follows:
1. The snake moves around a rectangular playing field bordered by walls, and with some optional internal walls.
2. The player uses arrow keys to change the snake’s direction.
3. Food appears at random locations on the playing field, and if the snake’s head hits the food, the snake eats it. The player’s score increases and new food appears elsewhere.
4. When the snake runs into a wall (or into its own body), it dies and the game is over.

To compile the game run in your terminal: 
'make clean all'

Next to play, run: 
'./snake 0' to run the game without a growing snake. 

'./snake 1' to run the game with a growing snake. 
(ensure that your terminal is at least 20 by 12 characters in size.)

## Collaborators: 
ayeung9, earth mokkamakkul
