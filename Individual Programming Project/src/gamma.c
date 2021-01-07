/** @file
 * IMplementation of the gamma game engine.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gamma.h"
#include "auxiliary_structs.h"


/** Initializes a queue..
 * @param [in, out] q - pointer to the pointer, which is to point to the initialized queue.
 */
static void queue_init(queue** q)
{
    *q = malloc(sizeof(queue));
    (*q)->beginning = NULL;
}


/** @brief Inserts a field intwo the queue.
 * Inserts into the queue a pair of coefficients new_width, new_height.
 * @param[in, out] q - the address of the queue,
 * @param[in] new_width - inserted column number, positive integer,
 * @param[in] new_height - inserted row number, positive integer
 */
static void queue_push(queue* q, uint32_t new_width, uint32_t new_height)
{
    field_list* new_field = malloc(sizeof(field_list));
    new_field->next = NULL;
    new_field->width_x = new_width;
    new_field->height_y = new_height;
    if(q->beginning == NULL)
    {
        q->beginning = new_field;
        q->end = new_field;
    }
    else
    {
        q->end->next = new_field;
        q->end = q->end->next;
    }
}

/** Checks if the queue is empty.
 * @param[in] q - the address of the queue,
 * @return True, if the queue is empty, and false 
 *         otherwise.
 */
static bool queue_empty(queue* q)
{
    return (q->beginning == NULL);
}

/** Pops the first element of the queue.
 * If it is empty, return NULL.
 * @param[in] q - the address of the queue.
 * @return pointer to the struct field_list containing the first queue element,
 * or NULL, if it is empty.
 */
static field_list* queue_pop(queue* q)
{
    if (queue_empty(q)) return NULL;
    field_list* aux = q->beginning;
    q->beginning = aux->next;
    aux->next = NULL;
    return aux;
}

/** Clears the queue.
 * @param[in] q - the address of the queue.
 */
static void queue_clear(queue** q)
{
    while(!queue_empty(*q))
    {
        field_list* aux = queue_pop(*q);
        free(aux);
    }
    free(*q);
    *q = NULL;
}

/** Creates a new array of players.
 * @param[in] players - array size (number of players), positive integer.
 * @return pointer to the allocated array.
 */
static player_t** new_arr_of_players(uint32_t players)
{
    player_t** new_arr = malloc(players*sizeof(player_t*));
    if(new_arr == NULL) return NULL;
    for(uint32_t i = 0; i < players; i++)
    {
        player_t* current = malloc(sizeof(player_t));
        if (current == NULL)
        {
            for(uint32_t j = 0; j < i; j++) free(new_arr[j]);
            free(new_arr);
            return NULL;
        }
        current->golden_performed = false;
        current->occupied_areas = 0;
        current->occupied_fields = 0;
        new_arr[i] = current;
    }
    return new_arr;
}

/** @brief Creates a new board.
 * @param[in] width - number of columns, positive integer,
 * @param[in] height - number of rowsy, positive integer.
 * @return The pointer to the allocated board.
 */
static uint32_t** new_board(uint32_t width, uint32_t height)
{
    uint32_t** board = malloc(width*sizeof(uint32_t*));
    if(board == NULL) return NULL;
    for(uint32_t i = 0; i < width; i++)
    {
        uint32_t* new_column = malloc(height*sizeof(uint32_t));
        if(new_column == NULL)
        {
            for(uint32_t j = 0; j < i; j++) free(board[j]);
            free(board);
            return NULL;
        }
        for(uint32_t j = 0; j < height; j++)
        {
            new_column[j] = 0;
        }
        board[i] = new_column;
    }
    return board;
}

/** @brief Creates an auxiliary board.
 * @param[in] width - number of columns, positive integer,
 * @param[in] height - number of rowsy, positive integer.
 * @return pointer to the allocated array.
 */
static bool** new_auxiliary(uint32_t width, uint32_t height)
{
    bool** auxiliary = malloc(width*sizeof(bool*));
    if(auxiliary == NULL) return NULL;
    for(unsigned int i = 0; i < width; i++)
    {
        bool* new_column = calloc(height, sizeof(bool));
        if (new_column == NULL)
        {
            for(uint32_t j = 0; j < i; j++) free(auxiliary[j]);
            free(auxiliary);
            return NULL;
        }
        auxiliary[i] = new_column;
    }
    return auxiliary;
}

/** Frees the array of players.
 * @param[in] target - pointer to the array of players.
 * @param[in] size - the size of the array.
 */
static void free_array_of_players(player_t** target, uint32_t size)
{
    if(target == NULL) return;
    for(uint32_t i = 0; i < size; i++)
    {
        player_t* p = target[i];
        free(p);
        target[i] = NULL;
    }
}

/** Frees the board.
 * @param[in] board - pointer to the array storing the board state.
 * @param[in] width - number of the columns of the array.
 */
static void free_board(uint32_t** board, uint32_t width)
{
    if(board == NULL) return;
    for(uint32_t i = 0; i < width; i++)
    {
        free(board[i]);
    }
}

/** Frees the auxiliary array.
 * @param[in] target - pointer to the auxiliary array.
 * @param[in] width - number of the array's columns.
 */
static void free_array_of_bools(bool** target, uint32_t width)
{
    if(target == NULL) return;
    for(uint32_t i = 0; i < width; i++)
    {
        bool* b = target[i];
        free(b);
        target[i] = NULL;
    }
}

/** @brief Clears the auxiliary array.
 * @param[in, out] g - pointer to the struct storing the game state.
 */
static void clear_aux(gamma_t* g)
{
    uint32_t width = g->width_x;
    uint32_t height = g->height_y;
    bool** target = g->auxiliary;
    for(uint32_t i = 0; i < width; i++)
    {
        for(uint32_t j = 0; j < height; j++) target[i][j] = 0;
    }
}

/** Checks if the field with coefficients x, y should be inserted into the queue
 * for the BFS algorithm seeking a path of adjacent fields belonging to a given player.
 * The result is positive if the player own the field and it has not yet been put in the BFS
 * queue, which is checked with help of the auxiliary array.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] x - the column number, non-negative integer smaller than the value of @p width
                  from the function @ref gamma_new,
 * @param[in] y - the row number, non-negative integer smaller than the value of @p height
                  from the function @ref gamma_new,
 * @param[in] player - player number, positive integer not bigger than the value of
 *                     @p players from the function @ref gamma_new.
 * @return True, if the field should be inserted into the queue, and false
 *         otherwise.
 */
static bool field_eligible(gamma_t* g, uint32_t x, uint32_t y, uint32_t player)
{
    if(g->auxiliary[x][y] == 1) return false;
    if(g->board[x][y] != player) return false;
    return true;
}

/** Auxiliart function for ther function @ref reachable, using the BFS algorithm.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] q- pointer to the queue for performing the algorithm,
 * @param[in] x - the column number of the target field,, non-negative integer equal to the value of
 *                @p x2 from the function @ref reachable,
 * @param[in] y - the row number of the target field,, non-negative integer equal to the value of
 *                @p y2 from the function @ref reachable,
 * @param[in] player  - player number, positive integer equal to the value of @p player from the
 *                      function @ref reachable,
 * @return True, if the path sought for by the function @ref reachable
 * exists, and false otherwise.
 */
static bool reachable_aux(gamma_t* g, queue* q, uint32_t x, uint32_t y, uint32_t player)
{
    while(!queue_empty(q))
    {
        field_list* f = queue_pop(q);
        if(f->width_x == x && f->height_y == y)
        {
            free(f);
            return true;
        }
        else
        {
            uint32_t width = f->width_x;
            uint32_t height = f->height_y;
            g->auxiliary[width][height] = 1;
            if(width != 0 && field_eligible(g, width-1, height, player))
            {
                queue_push(q, width-1, height);
            }
            if(height != 0 && field_eligible(g, width, height-1, player))
            {
                queue_push(q, width, height-1);
            }
            if(width != UINT32_MAX && width != g->width_x-1 && field_eligible(g, width+1, height, player))
            {
                queue_push(q, width+1, height);
            }
            if(height != UINT32_MAX && height != g->height_y -1 && field_eligible(g, width, height+1, player))
            {
                queue_push(q, width, height+1);
            }
            free(f);
        }
    }
    return false;
}

/** @brief Checks if two fields can be reached from one another through a path of adjacent fields
 * belonging to one player. 
 * @param[in,out] g -pointer to the struct storing the game state,
 * @param[in] x1 - the column number of the first field, non-negative integer smaller than the value of
 *                 @p width from the function @ref gamma_new,
 * @param[in] y1 - the row number of the first field, non-negative integer smaller than the value of
 *                 @p width from the function @ref gamma_new,
 * @param[in] x2 - the column number of the second field, non-negative integer smaller than the value of
 *                 @p width from the function @ref gamma_new,
 * @param[in] y2 - the row number of the second field, non-negative integer smaller than the value of
 *                 @p width from the function @ref gamma_new,
 * @param[in] player  - player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new.
 * @return True, if such a path exists, and false otherwise.
 */
static bool reachable(gamma_t* g, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t player)
{
    queue* q = NULL;
    queue_init(&q);
    queue_push(q, x1, y1);
    bool res = reachable_aux(g, q, x2, y2, player);
    clear_aux(g);
    queue_clear(&q);
    return res;
}

/** Does nothing, if on the list @p l there already is a field reachable from (x, y)
 * by a path of adjacent fields belonging to one player, otherwise appends
 * (x, y) to the beginning of that list.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] l - pointer to pointer to the beginning od the queue,
 * @param[in] x - the column number, non-negative integer smaller than the value of
 *                 @p width from the function @ref gamma_new,
 * @param[in] y - the row number, non-negative integer smaller than the value of
 *                 @p width from the function @ref gamma_new,
 * @param[in] player  - player number, positive integer not bigger than the value of @p players from the function @ref gamma_new.
 */
static void append_field(gamma_t* g, field_list** l, uint32_t x, uint32_t y, uint32_t player)
{
    field_list* temp = *l;
    if(*l == NULL)
    {
        field_list* new_field = malloc(sizeof(field_list));
        new_field->next = NULL;
        new_field->width_x = x;
        new_field->height_y = y;
        *l = new_field;
    }
    else
    {
        while(*l != NULL && !reachable(g, x, y, (*l)->width_x, (*l)->height_y, player))
        {
            *l = (*l)->next;
        }
        if(*l == NULL)
        {
            field_list* new_list = malloc(sizeof(field_list));
            new_list->width_x = x;
            new_list->height_y = y;
            new_list->next = temp;
            *l = new_list;
        }
        else
        {
            *l = temp;
        }
    }
}

/** Returns the length of the list @p *l and frees that list.
 * @param[in] l - pointer to pointer to the beginning od the queue.
 */
static unsigned int field_list_length(field_list** l)
{
    unsigned int i = 0;
    while(*l != NULL)
    {
        i++;
        field_list* temp = *l;
        *l = (*l)->next;
        free(temp);
    }
    return i;
}

/** @brief Checks, how many areas of a given player that the fields adjacent to ( @p x, @p y) belong to.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] x - the column number, non-negative integer smaller than the value of
 *                 @p width from the function @ref gamma_new,
 * @param[in] y - the row number, non-negative integer smaller than the value of
 *                 @p width from the function @ref gamma_new,
 * @param[in] player  - player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new.
 * @return The number of areas of a given player that the fields adjacent to ( @p x, @p y) belong to. 
 */
static unsigned int adjacent_player_areas(gamma_t* g, uint32_t x, uint32_t y, uint32_t player)
{
    uint32_t** board = g->board;
    field_list* list = NULL;
    if(x != 0 && board[x-1][y] == player)
    {
        append_field(g, &list, x-1, y, player);
    }
    if(y != 0 &&  board[x][y-1] == player)
    {   
        append_field(g, &list, x, y-1, player);
    }
    if(x != UINT32_MAX && x != g->width_x - 1 && board[x+1][y] == player)
    {
        append_field(g, &list, x+1, y, player);
    }
    if(y != UINT32_MAX && y != g->height_y - 1 && board[x][y+1] == player)
    {
        append_field(g, &list, x, y+1, player);
    }
    return field_list_length(&list);
}

/** Returns the number of free fields on the board, adjacent to at least one field
 * belonging to a given player.
 * @param[in] g   - pointer to the struct storing the game state,
 * @param[in] player  - player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new.
 * @return The number of free fields on the board, adjacent to at least one field
 * belonging to a given player.
 */
static uint64_t fields_adjacent_to_player(gamma_t* g, uint32_t player)
{
    uint32_t width = g->width_x;
    uint32_t height = g->height_y;
    uint32_t** board = g->board;
    unsigned int res = 0;
    for(uint32_t i = 0; i < width; i++)
    {
        for(uint32_t j = 0; j < height; j++)
        {
            if(board[i][j] == 0 && adjacent_owned_by_player(g, i, j, player)) res++;
        }
    }
    return res;
}

/** Returns the ascii value of the digit corresponding to the number x.
 * @param[in] x - non-negative integer smaller or equal to 9.
 * @return The ascii value of the digit corresponding to the number x.
 */
static char number_to_digit(int x)
{
    return (char) x + 48;
}

/** @brief Writes the decimal representation of a number into an array.
 * @param[in, out] beginning - pointer to the target place in the array,
 * @param[in] total_length - the total length of the number's representation
 *                           and the preceding spaces,
 * @param[in] x - the number to be written,
 * @param[in] white - the white character appended at the end.
 */
static void write_number(char* beginning, unsigned int total_length, uint32_t x, char white)
{
    if(x == 0)
    {
        for(unsigned int i = 0; i < total_length-1; i++) beginning[i] = ' ';
        beginning[total_length-1] = '.';
    }
    else
    {
        unsigned int x_length = decimal_length(x);
        unsigned int num_of_spaces = total_length - x_length;
        for(unsigned int i = 0; i < num_of_spaces; i++) beginning[i] = ' ';
        for(unsigned int j = total_length-1; x > 0; j--)
        {
            beginning[j] = number_to_digit(x%10);
            x /= 10;
        }
    }
    beginning[total_length] = white;
}

/** Creates the string storing the board state in case
 * not all player numbers are single-digit numbers.
 @param[in] g - pointer to the struct storing the game state,
 @param[in] max_digits - maximal length of the representation of a player number.
 @return pointer to the resulting buffer.
 */
static char* fill_buffer_with_spaces(gamma_t* g, unsigned int max_digits)
{
    char* buffer = malloc(((max_digits+1)*g->width_x* (uint64_t) g->height_y + 1)*sizeof(char));
    char* result = buffer;
    if(buffer == NULL) return NULL;
    uint32_t** board = g->board;
    uint32_t width = g->width_x;
    for(uint32_t y = g->height_y; y > 0; y--)
    {
        for(uint32_t x = 0; x < width-1; x++)
        {
            write_number(buffer, max_digits, board[x][y-1], ' ');
            buffer += max_digits + 1;
        }
        write_number(buffer, max_digits, board[width-1][y-1], '\n');
        buffer += max_digits + 1;
    }
    buffer[0] = '\0';
    return result;
}

/** @brief Creates the string storing the board state in case
 * all player numbers are single-digit numbers. Fields are not
 * separated by spaces in this case.
 @param[in] g - pointer to the struct storing the game state,
 @return pointer to the resulting buffer.
 */
static char* fill_buffer_without_spaces(gamma_t* g)
{
    char* buffer = malloc(((g->width_x + 1) * (uint64_t) g->height_y + 1)* sizeof(char));
    char* result = buffer;
    if(buffer == NULL) return NULL;
    uint32_t** board = g->board;
    uint32_t width = g->width_x;
    unsigned int owner_num;
    for(uint32_t y = g->height_y; y > 0; y--)
    {
        for(uint32_t x = 0; x < width; x++)
        {
            owner_num = board[x][y-1];
            if(owner_num != 0) buffer[0] = number_to_digit(owner_num);
            else buffer[0] = '.';
            buffer++;
        }
        buffer[0] = '\n';
        buffer++;
    }
    buffer[0] = '\0';
    return result;
}

/** Auxiliary function of @ref gamma_move, setting a player's checker on a
 *  given field and changing the value of the number of free fields and field occupied_areas
 * by the player.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] x - the column number, non-negative integer smaller than the value of
 *                @p width from the function @ref gamma_new,
 * @param[in] y - the row number, non-negative integer smaller than the value of
 *                @p height from the function @ref gamma_new,
 * @param[in] p - pointer to the struct storing the player,
 * @param[in] player  - player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new.
 */
static void add_field(gamma_t* g, uint32_t x, uint32_t y, player_t* p, uint32_t player)
{
    p->occupied_fields += 1;
    g->board[x][y] = player;
    g->free_fields -= 1;
}

bool adjacent_owned_by_player(gamma_t *g, uint32_t x, uint32_t y, uint32_t player)
{
    uint32_t** board = g->board;
    if(x != 0 && board[x-1][y] == player) return true;
    if(y != 0 && board[x][y-1] == player) return true;
    if(x != UINT32_MAX && x != g->width_x - 1 && board[x+1][y] == player) return true;
    if(y != UINT32_MAX && y != g->height_y - 1 && board[x][y+1] == player) return true;
    return false;;
}

unsigned int decimal_length(uint32_t x)
{
    unsigned int res = 1;
    while (x >= 10)
    {
        x /= 10;
        res++;
    }
    return res;
}

gamma_t* gamma_new(uint32_t width, uint32_t height, uint32_t players, uint32_t areas)
{
    if(width == 0 || height == 0 || players == 0 || areas == 0) return NULL;
    gamma_t* newgamma = malloc(sizeof(gamma_t));
    if (newgamma == NULL) return NULL;
    newgamma->width_x = width;
    newgamma->height_y = height;
    newgamma->n_of_players = players;
    newgamma->n_of_areas = areas;
    newgamma->free_fields = width*height;
    newgamma->arr_of_players = new_arr_of_players(players);
    if (newgamma->arr_of_players == NULL)
    {
        free(newgamma);
        return NULL;
    }
    newgamma->board = new_board(width, height);
    if (newgamma->board == NULL)
    {
        free_array_of_players(newgamma->arr_of_players, players);
        free(newgamma->arr_of_players);
        free(newgamma);
        return NULL;
    }
    newgamma->auxiliary = new_auxiliary(width, height);
    if (newgamma->auxiliary == NULL)
    {
        free_array_of_players(newgamma->arr_of_players, players);
        free(newgamma->arr_of_players);
        free_board(newgamma->board, width);
        free(newgamma->board);
        free(newgamma);
        return NULL;
    }
    return newgamma;
}

void gamma_delete(gamma_t *g)
{
    if(g == NULL) return;
    free_array_of_players(g->arr_of_players, g->n_of_players);
    free(g->arr_of_players);
    g->arr_of_players = NULL;
    free_board(g->board, g->width_x);
    free(g->board);
    g->board = NULL;
    free_array_of_bools(g->auxiliary, g->width_x);
    free(g->auxiliary);
    g->auxiliary = NULL;
    free(g);
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y)
{
    if(g == NULL) return false;
    if(player == 0 || player > g->n_of_players) return false;
    if(x >= g->width_x) return false;
    if(y >= g->height_y) return false;
    uint32_t** board = g->board;
    if(board[x][y] != 0) return false;
    player_t* p = g->arr_of_players[player-1];
    unsigned int areas = adjacent_player_areas(g, x, y, player);
    if (areas == 0) // tworzy sie nowy obszar nalezacy do gracza
    {
        if( p->occupied_areas > g->n_of_areas - 1 || g->n_of_areas == 0)
        {
            return false;
        }
        else
        {
            p->occupied_areas += 1;
            add_field(g, x, y, p, player);
            return true;
        }
    }
    else
    {
        p->occupied_areas -= areas - 1;
        add_field(g, x, y, p, player);
        return true;
    }
}

bool golden_possible_on_field(gamma_t* g, player_t* new_owner, uint32_t new_owner_num, uint32_t x, uint32_t y)
{
    uint32_t prev_owner_num = g->board[x][y];
    if(prev_owner_num == 0 || prev_owner_num == new_owner_num) return false;
    g->board[x][y]= 0;
    unsigned int adjacent_prev_owner_areas = adjacent_player_areas(g, x, y, prev_owner_num);
    player_t* prev_owner = g->arr_of_players[prev_owner_num-1];
    bool disintegrating = false;
    if(adjacent_prev_owner_areas != 0)
        if(adjacent_prev_owner_areas - 1 > g->n_of_areas - prev_owner->occupied_areas)
            disintegrating = true;
    if(disintegrating)
    {
        g->board[x][y] = prev_owner_num;
        return false;
    }
    unsigned int adjacent_new_owner_areas = adjacent_player_areas(g, x, y, new_owner_num);
    if(adjacent_new_owner_areas == 0 && new_owner->occupied_areas == g->n_of_areas)
    {
        g->board[x][y] = prev_owner_num;
        return false;
    }
    g->board[x][y] = prev_owner_num;
    return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y)
{
    if(g==NULL) return false;
    if(player == 0 || player > g->n_of_players) return false;
    if(x >= g->width_x || y >= g->height_y) return false;
    player_t* new_owner = g->arr_of_players[player-1];
    if(new_owner->golden_performed == true) return false;
    uint32_t** board = g->board;
    uint32_t prev_owner_num = board[x][y];
    if(prev_owner_num == 0 || prev_owner_num == player) return false;
    player_t* prev_owner = g->arr_of_players[prev_owner_num-1];
    board[x][y]= 0;
    unsigned int adjacent_prev_owner_areas = adjacent_player_areas(g, x, y, prev_owner_num);
    board[x][y] = prev_owner_num;
    bool can_move = golden_possible_on_field(g, new_owner, player, x, y);
    if(can_move)
    {
        unsigned int adjacent_new_owner_areas = adjacent_player_areas(g, x, y, player);
        board[x][y] = player;
        new_owner->occupied_areas -= adjacent_new_owner_areas - 1;
        new_owner->occupied_fields += 1;
        new_owner->golden_performed = true;
        prev_owner->occupied_fields -= 1;
        prev_owner->occupied_areas += adjacent_prev_owner_areas - 1;
        return true;
    }
    else return false;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player)
{
    if (g == NULL) return 0;
    if (player > g->n_of_players || player == 0) return 0;
    return g->arr_of_players[player-1]->occupied_fields;
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player)
{

    if(g == NULL) return 0;
    if(player == 0 || player > g->n_of_players) return 0;
    player_t* target_player = g->arr_of_players[player-1];
    if(target_player->occupied_areas == g->n_of_areas) return fields_adjacent_to_player(g, player);
    else return g->free_fields;
}

bool gamma_golden_possible(gamma_t *g, uint32_t player)
{
    if(g == NULL) return false;
    if(player > g->n_of_players) return false;
    player_t* target = g->arr_of_players[player-1];
    uint64_t occupied_by_others = (uint64_t) g->width_x * g->height_y - g->free_fields - target->occupied_fields;
    if(target->golden_performed || occupied_by_others == 0) return false;
    else
    {
        for(uint32_t i = 0; i < g->width_x; i++)
            for(uint32_t j = 0; j < g->height_y; j++)
            {
                if(golden_possible_on_field(g, target, player, i, j)) return true;
            }
    }
    return false;
}

char* gamma_board(gamma_t *g)
{
    if(g == NULL) return NULL;
    unsigned int max_digits = decimal_length(g->n_of_players);
    if(max_digits != 1)
    {
        return fill_buffer_with_spaces(g, max_digits);
    }
    else
    {
        return fill_buffer_without_spaces(g);
    }
}

