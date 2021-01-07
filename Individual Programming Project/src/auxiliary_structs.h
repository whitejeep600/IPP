/** @file
 * Auxiliary structures for the engine of the gamma game.
 */
 
 #ifndef STRUCTS_H
 #define STRUCTS_H
 
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


/** Struct that stores a player.
 */
typedef struct player_s
{
    uint64_t occupied_fields; ///< number of fields occupied by the player, non-negative integer
    uint32_t occupied_areas; ///<  number of areas occupied by the player, non-negative integer
    bool golden_performed; ///< boolean value informing if the player has performed their golden move
} player_t;

/** Struct that stores the game state.
 */
typedef struct gamma
{
    uint32_t width_x; ///< board width, positive integer
    uint32_t height_y; ///< board height, positive integer
    uint32_t n_of_players; ///< number of players, positive integer
    uint32_t n_of_areas; ///< maximum number of areas, positive integer
    uint64_t free_fields; ///< number of free fields, non-negative integer
    player_t** arr_of_players; ///< array of players
    uint32_t** board; ///< array storing the state of the board
    bool** auxiliary; ///< auxiliary array for executing the BFS algorithm.
} gamma_t;

/** A list of field coefficient pairs.
 */
typedef struct field_list_s
{
    uint32_t width_x; ///< column number, non-negative integer
    uint32_t height_y; ///< row number, non-negative integer
    struct field_list_s* next; /**< pointer to the next list element or null,
                                    if the given element is the last one.**/
} field_list;

/** Struct that implements a queue.
 */
typedef struct queue_s
{
    field_list* beginning; ///< pointer to a field_list_s struct which is the beginning of the queue
    field_list* end; ///< pointer to a field_list_s struct which is the end of the queue
} queue;

/** Enum for storing the possible commands in batch mode :
 *  move, golden move, function @ref gamma_busy_fields,
 *  function @ref gamma_free_fields, function @ref gamma_golden_possible, function
 *  @ref gamma_board.
 */
enum command_type{gmove, golden, busy, freef, possible, board};

/** Struct that stores a batch-mode command.
*/
typedef struct game_command_s
{
    enum command_type type; ///< command type
    uint32_t player_no; ///< player number, for whom the command is executed
    uint32_t x_co; ///< x coefficient
    uint32_t y_co; ///< y coefficient
} game_command;

/**  Enum for storing the possible commands in interactive mode :
 *  arrows, move skip, game end, regular move, golden move, another key
 *  which is not a command.
 */
enum key{up, down, left, right, skip, end, spacebar, golden_g, other};

/** Struct that stores the coefficients of the virtual cursor (according to the board, and not the terminal)
 * as well as the key data necessary for determining the coefficients in the terminal based on them.
 */
typedef struct cursor_s
{
    uint32_t x; ///< x coefficient
    uint32_t y; ///< y coefficient
    uint32_t height; ///< total board height
    unsigned int field_width; ///< field width
}cursor_t;
 
#endif // STRUCTS_H
