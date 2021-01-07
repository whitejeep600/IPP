/** @file
 * Implementation of the functions responsible for interactive mode playing.
 */

#include <stdio.h>
#include "interactive_mode.h"

/** Prints the word "pole" (field) with an appropriate Polish ending
 *  depending on the number and grammatical case (the game was designed
 *  as a part of an assigment from a Polish university, so this was necessary).
 *  Incidentally, some nice peculiarities of the Polish grammar have to be
 *  accounted for here :) 
 *  @param[in] n - the number.
 */
static void polish_ending_field(uint64_t n)
{
    switch(n % 10)
    {
        case 2: case 3: case 4:
            printf("pola");
            break;
        case 0: case 5: case 6: case 7: case 8: case 9:
            printf("pol");
            break;
        case 1:
            if(n == 1) printf("pole");
            else printf("pol");
            break;
        default:
            break;
    }
}

/** Prints a move prompt.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] player - player number, positive integer,
 * @param[in] cursor - pointer to the struct storing the program cursor.
 */
static void print_prompt(gamma_t* g, uint32_t player, cursor_t* cursor)
{
    set_cursor_to_line(g->height_y + 1);
    player_t* prompted_player = g->arr_of_players[player-1];
    printf("Tura gracza o numerze %" PRIu32 "\n", player);
    uint64_t total_fields = g->width_x*g->height_y;
    printf("Zajete pola: %" PRIu64 "" , gamma_busy_fields(g, player));
    printf("/%" PRIu64 "\n", total_fields);
    printf("Zajete obszary: %" PRIu32 "" , prompted_player->occupied_areas);
    printf("/%" PRIu32 "\n", g->n_of_areas);
    uint64_t free = gamma_free_fields(g, player);
    if(free == 0) printf("Nie ma wolnych pol, na ktore gracz moze sie ruszyc\n");
    else
    {
        printf("W nastepnym moveu gracz moze sie ruszyc na ");
        printf("%" PRIu64 " ", free);
        polish_ending_field(free);
        printf("\n");
    }
    if(prompted_player->golden_performed)
    {
        printf("golden move zostal juz wykonany\n");
    }
    else if(gamma_golden_possible(g, player))
    {
        printf("golden move moze zostac wykonany (zolte pola)\n");
    }
    else
    {
        printf("golden move nie moze zostac wykonany\n");
    }
    set_cursor_on_board(cursor);
}

/** Moves the virtual cursor one field to the left, provided that this is possible.
 * @param[in, out] cursor - pointer to the struct storing the game cursor.
 */
static void execute_left_arrow(cursor_t* cursor)
{
    if(cursor->x == 0) return;
    else cursor->x -= 1;
}

/** Moves the virtual cursor one field to the right, provided that this is possible.
 * @param[in, out] cursor - pointer to the struct storing the game cursor,
 * @param[in] g - pointer to the struct storing the game state.
 */
static void execute_right_arrow(gamma_t* g, cursor_t* cursor)
{
    if(cursor->x == g->width_x - 1) return;
    else cursor->x += 1;
}

/** Moves the virtual cursor one field up, provided that this is possible.
 * @param[in, out] cursor - pointer to the struct storing the game cursor,
 * @param[in] g - pointer to the struct storing the game state.
 */
static void execute_up_arrow(gamma_t* g, cursor_t* cursor)
{
    if(cursor->y == g->height_y - 1) return;
    else cursor->y += 1;
}

/** Moves the virtual cursor one field down, provided that this is possible.
 * @param[in, out] cursor - pointer to the struct storing the game cursor.
 */
static void execute_down_arrow(cursor_t* cursor)
{
    if(cursor->y == 0) return;
    else cursor->y -= 1;
}

/** Moves the virtual cursor by one field in the given direction, provided that this is possible.
 * @param[in] cursor - pointer to the struct storing the game cursor,
 * @param[in] k - enum determining the pressed arrow key,
 * @param[in] g - pointer to the struct storing the game state.
 */
static void execute_arrow(gamma_t* g, enum key k, cursor_t* cursor)
{
    switch(k)
    {
        case left:
            execute_left_arrow(cursor);
            break;
            
        case right:
            execute_right_arrow(g, cursor);
            break;
            
        case up:
            execute_up_arrow(g, cursor);
            break;
            
        case down:
            execute_down_arrow(cursor);
            break;
        
        default:
            break;
    }
    set_cursor_on_board(cursor);
}

/** Return the enum determining the program command corresponding to the command
 * which should be read if the last character read was 'c', and the combination 
 * esc+[ has not been pressed.
 * @param[in] c - the last character read.
 * @return The according command if the combination esc+[ has not been read.
 */
static enum key without_esc(char c)
{
    switch(c)
    {
        case 4:
        case EOF:
            return end;
        case 67:
        case 99:
            return skip;
        case 32:
            return spacebar;
        case 71:
        case 103:
            return golden_g;
        default:
            return other;
    }
}

/** Returns enum determining the program command that should be read if the
 *  previous read characters were esc + [, and the last one was 'c'. This includes
 *  reading arrow keys. For other characters, it behaves like the function @ref without_esc.
 * @param[in] c - the last read character.
 * @return the program command corresponding to the command read for a given character after
 * the esc+[ combination.
 */
static enum key after_esc(char c)
{
    switch(c)
    {
        case 68:
            return left;
        case 67:
            return right;
        case 65:
            return up;
        case 66:
            return down;
        case EOF:
            return end;
        default:
            return without_esc(c);
    }
}

/** Reads a character from the input and returns an enum determining
 *  either a correct interactive mode command, corresponding to the read character,
 *  or an ignored character.
 * @return The read key in the form of enum key.
 */
static enum key get_key()
{
    char c = getchar();
    bool has_esc = false;
    if(c == 27)
    {
        has_esc = true;
        while(c == 27) c = getchar();
    }
    if(c == '[')
    {
        if(has_esc)
        {
            c = getchar();
            return after_esc(c);
        }
        return other;

    }
    return without_esc(c);
}

/** Returns the char corresponding to the digit which in turn corresponds to the 'player' number.
 * @param[in] player - positive integer smaller than 10.
 * @return The character corresponding to the 'player' number.
 */
static char to_char(uint32_t player)
{
    return (char) player + 48;
}

/** Determines the index in the string describing the board state, where the representation of
 *  the field pointed to by the virtual cursor begins.
 * @param[in, out] g - pointer to the struct storing the game state.
 * @param[in] cursor - pointer to the struct storing the program cursor.
 * @return The number determining the index where the representation of
 *  the field pointed to by the virtual cursor begins.
 */
static uint64_t give_start_index(gamma_t* g, cursor_t* cursor)
{
    uint64_t res = g->width_x * (g->height_y - cursor->y - 1) + cursor->x;
    res *= cursor->field_width;
    return res;
}

/** Returns, as a character, the last digit of the number pointed to by the 'player' pointer,
 * or space, if this digit is equal to 0.
 * @param[in, out] player - pointer to a non-negative integer.
 * @return The character corresponding to the last digit of the number pointed to by the 'player'
 * pointer, or space, if this digit is equal to 0.
 */
static char next_dig(uint32_t* player)
{
    if(*player == 0) return ' ';
    else
    {
        unsigned int temp = (*player) % 10;
        *player /= 10;
        return to_char(temp);
    }
}

/** Reprints the board field that is pointed to by the virtual game cursor.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] cursor - pointer to the struct storing the virtual cursor,
 * @param[in] board_image - pointer to the string storing the board state.
 */
static void reprint_field(gamma_t* g, cursor_t* cursor, char* board_image)
{
    if(cursor->field_width == 1)
    {
        uint64_t a = (g->width_x+1) * (g->height_y - cursor->y - 1 ) + cursor->x;
        printf("%c", board_image[a]);
        cursor_left(1);
        return;
    }
    else
    {
        uint64_t start = give_start_index(g, cursor);
        unsigned int y = cursor->field_width;
        cursor_left(y - 2);
        for(unsigned int i = 0; i < y-1; i++) printf("%c", board_image[start+i]);
        cursor_left(1);
    }
}

/** Modifies the string storing the board state so that the field pointed to by
 * the virtual cursor is owned by the 'player' player.
 * @param[in, out] g - pointer to struct describing the game state,
 * @param[in, out] board_image - pointer to the string storing the board state,
 * @param[in] player - player number, positive integer,
 * @param[in] cursor - pointer to the struct describing the virtual cursor.
 */
static void change_board(gamma_t* g, char* board_image, uint32_t player, cursor_t* cursor)
{
    if(cursor->field_width == 1)
    {
        uint64_t a = (g->width_x+1) * (g->height_y - cursor->y - 1 ) + cursor->x;
        board_image[a] = to_char(player);
    }
    else
    {
        uint64_t start = give_start_index(g, cursor);
        uint64_t current_index = start + cursor->field_width - 1;
        while(current_index  > start)
        {
            board_image[current_index-1] = next_dig(&player);
            current_index--;
        }
    }
}

/** Highlights the board field which is pointed to by the virtual cursor, in the appropriate
 * color depending on the currently moving player.
 * @param[in, out] g - pointer to the struct describing the game state,
 * @param[in] cursor - pointer to the struct describing the virtual cursor,
 * @param[in] player - player number, positive integer,
 * @param[in] board_image - pointer to the string storing the board state.
 */
static void highlight_for_player(gamma_t* g, cursor_t* cursor, uint32_t player, char* board_image)
{
    player_t* current_player = g->arr_of_players[player-1];
    uint32_t cursor_x = cursor->x;
    uint32_t cursor_y = cursor->y;
    bool changed_attributes = false;
    if(golden_possible_on_field(g, current_player, player, cursor_x, cursor_y) &&
       !current_player->golden_performed)
    {
        background_to_yellow();
        changed_attributes = true;
    }
    else if(g->board[cursor_x][cursor_y] == player)
    {
        background_to_red();
        changed_attributes = true;
    }
    else if(adjacent_owned_by_player(g, cursor_x, cursor_y, player) &&
            g->board[cursor_x][cursor_y] == 0)
    {
        background_to_magenta();
        changed_attributes = true;
    }
    if(changed_attributes) foreground_to_black();
    else reverse_video();
    reprint_field(g, cursor, board_image);
    reset_video();
}

/** Reprints the field pointed to by thevirtual game cursor,
 *  in the color corresponding to the currently moving player.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] player - player number, positive integer,
 * @param[in] cursor - pointer to the struct storing the virtual cursor,
 * @param[in] board_image - pointer to the string storing the board state.
 */
static void reprint_field_for_player(gamma_t* g, uint32_t player, char* board_image, cursor_t* cursor)
{
    player_t* current_player = g->arr_of_players[player-1];
    uint32_t cursor_x = cursor->x;
    uint32_t cursor_y = cursor->y;
    if(golden_possible_on_field(g, current_player, player, cursor->x, cursor->y))
    {
        if(!current_player->golden_performed) foreground_to_yellow();
    }
    else if(g->board[cursor_x][cursor_y] == player)
    {
        foreground_to_red();
    }
    else if(adjacent_owned_by_player(g, cursor_x, cursor_y, player) &&
            g->board[cursor_x][cursor_y] == 0)
    {
        foreground_to_magenta();
    }
    reprint_field(g, cursor, board_image);
    reset_video();
}

/** Prints the board after the game is over.
 * @param[in] board_image - pointer to the string storing the board state,
 * @param[in] cursor - pointer to the struct storing the program cursor.
 */
static void no_player_show_board(char* board_image, cursor_t* cursor)
{
    set_cursor_on_coeffs(1, 1);
    printf(board_image);
    set_cursor_on_board(cursor);
}

/** Prints the board before a player's move.
 * @param[in] g - pointer to the struct describing the game state,
 * @param[in] player - number of the currently moving player,
 * @param[in] board_image - pointer to the string storing the board state,
 * @param[in] cursor - pointer to the struct storing the program cursor.
 */
static void show_board_for_player(gamma_t* g, uint32_t player, char* board_image, cursor_t* cursor)
{
    cursor_t* temp = malloc(sizeof(cursor_t));
    temp->height = cursor->height;
    uint32_t total_width = g->width_x;
    temp->field_width = cursor->field_width;
    bool is_not_zero = true;
    for(uint32_t i = cursor->height-1; is_not_zero; i--)
    {
        for(uint32_t j = 0; j < total_width; j++)
        {
            temp->y = i;
            temp->x = j;
            set_cursor_on_board(temp);
            reprint_field_for_player(g, player, board_image, temp);
        }
        if(i == 0) is_not_zero = false;
    }
    set_cursor_on_board(cursor);
}


/** Determines if a key is an arrow key.
 * @param[in] k - enum determining the key.
 * @return true, if the key is an arrow, and false otherwise.
 */
static bool is_arrow(enum key k)
{
    return k == left || k == right || k == up || k == down;
}

/** Determines if the key is executable by the program.
 * @param[in] k - enum determining the key.
 * @return true, if the key is executable, and false otherwise.
 */
static bool executable_key(enum key k)
{
    return k != other && !is_arrow(k);
}

/** Calls the gamma_move function or the gamma_golden_move function.
 * @param[in] k - enum determining the called command,
 * @param[in, out] g - pointer to the struct storing the game state,
 * @param[in] player - player number, positive integer,
 * @param[in] cursor - pointer to the struct storing the program cursor.
 * @return The return value of the appropriate function call.
 */
static bool execute_key(enum key k, gamma_t* g, uint32_t player, cursor_t* cursor)
{
    if(k == golden_g) return gamma_golden_move(g, player, cursor->x, cursor->y);
    else return gamma_move(g, player, cursor->x, cursor->y);
}

/** Sums up the game.
 * @param[in] g - pointer to the struct storing the game state.
 */
static void sum_up(gamma_t* g)
{
    set_cursor_to_line(g->height_y + 1);
    printf("\nKoniec gry.\n");
    reset_video();
    uint32_t num_of_players = g->n_of_players;
    uint64_t busy;
    uint64_t max_score = 0;
    uint32_t winners = 0;
    for(uint32_t i = 1; i <= num_of_players; i++)
    {
        busy = gamma_busy_fields(g, i);
        if(busy > max_score)
        {
            max_score = busy;
            winners = 1;
        }
        else if(busy == max_score) winners++;
    }
    if(winners == 1) printf("Mamy zwyciezce!\n");
    else printf("Najlepszy wynik osiagnelo remisowo %" PRIu32" graczy\n", winners);
    for(uint32_t i = 1; i <= num_of_players; i++)
    {
        busy = gamma_busy_fields(g, i);
        printf("Gracz %" PRIu32 " zajal %" PRIu64 " ", i, busy);
        polish_ending_field(busy);
        if(busy == max_score)
        {
            printf(" - zwyciezca!");
            if(winners != 1) printf(" Niestety tylko ex aequo.");
        }

        printf("\n");
    }
}

/** Executes a move or skip by a given player.
 * @param[in, out] g - pointer to the struct storing the game state,
 * @param[in, out] board_image - pointer to the string storing the board state,
 *                               modified after each move,
 * @param[in] current_player - the number of the currently moving player,
 * @param[in, out] cursor - pointer to the struct storing the virtual game cursor.
 * @return true, if ctrl+D was pressed, and false otherwise.
 */
static bool move(gamma_t* g, char* board_image, uint32_t current_player, cursor_t* cursor)
{
    show_board_for_player(g, current_player, board_image, cursor);
    print_prompt(g, current_player, cursor);
    highlight_for_player(g, cursor, current_player, board_image);
    enum key k;
    bool performed = false;
    bool has_game_end = false;
    while(!performed)
    {
        do
        {
            k = get_key();
            if(is_arrow(k))
            {
                reprint_field_for_player(g, current_player, board_image, cursor);
                execute_arrow(g, k, cursor);
                highlight_for_player(g, cursor, current_player, board_image);
            }
        }
        while(!executable_key(k));
        if(k == end)
        {
            has_game_end = true;
            performed = true;
        }
        else if(k == skip) performed = true;
        else performed = execute_key(k, g, current_player, cursor);
    }
    if(k == golden_g || k == spacebar)
    {
        change_board(g, board_image, current_player, cursor);
    }
    return has_game_end;
}

/** Determines if a player can execute a move.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] player - the player number, positive integer.
 * @return true, if the player can execute a move, and false otherwise.
 */
static bool can_move(gamma_t* g, uint32_t player)
{
    return gamma_free_fields(g, player) || gamma_golden_possible(g, player);
}

/** Returns the number of the next player.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] prev - player number of the previous player, positive integer.
 * @return positive integer which is the number of the next player.
 */
static uint32_t next_player_no(gamma_t* g, uint32_t prev)
{
    if(prev == g->n_of_players) return 1;
    else return prev + 1;
}

/** Returns the number of the next player that can execute a move.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] prev - the number of the previous player, positive integer.
 * @return The number of the next player who can execute a move, or 0, if there is no such player.
 */
static uint32_t next_moving_player(gamma_t* g, uint32_t prev)
{
    uint32_t p = prev;
    do
    {
        p = next_player_no(g, p);
    }
    while(!can_move(g, p) && p != prev);
    if(p == prev)
    {
        if(can_move(g, p)) return p;
        else return 0;
    }
    else return p;
}


void play_interactive(gamma_t* g, bool* mem_err)
{
    if(!correct_terminal(g))
    {
        printf("Okno terminala jest za male, aby rozpoczac gre.\n");
        return;
    }
    struct termios* normal_settings = malloc(sizeof(struct termios));
    if(normal_settings == NULL)
    {
        *mem_err = true;
        return;
    }
    struct termios* unbuff = malloc(sizeof(struct termios));
    if(unbuff == NULL)
    {
        *mem_err = true;
        free(normal_settings);
        return;
    }
    cursor_t* cursor = malloc(sizeof(cursor_t));
    if(cursor == NULL)
    {
        *mem_err = true;
        free(normal_settings);
        free(unbuff);
        return;
    }
    set_unbuff_input(normal_settings, unbuff);
    initialize_cursor(g, cursor);
    bool end_game = false;
    char* board_image = gamma_board(g); // will be edited during the game.
    clear_screen();
    uint32_t current_player = 1;
    do
    {
        end_game = move(g, board_image, current_player, cursor);
        clear_screen();
        current_player = next_moving_player(g, current_player);
    }
    while(current_player != 0 && end_game == false);
    no_player_show_board(board_image, cursor);
    sum_up(g);
    set_normal_input(normal_settings);
    free(cursor);
    free(board_image);
}
