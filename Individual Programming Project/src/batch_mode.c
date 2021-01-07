/** @file
 * Implementation of the functions responsible for executing the batch mode.
 *
 */
 
#include "batch_mode.h"
#include "parsing.h"


/** Saves into the struct storing a batch-mode command the move command.
 * @param[in, out] target - pointer to the target structure,
 * @param[in] source - pointer to the array of strings determining the command parameters.
 * @return true, if the command has been saved correctly, and false otherwise.
 */
static bool write_gmove(game_command* target, char** source)
{
    target->type = gmove;
    if(source[3] == NULL || source[4] != NULL) return false;
    for(int i = 1; i <= 3; i++) if(!string_is_digit(source[i])) return false;
    uint32_t a;
    uint64_t b = strtoul(source[1], NULL, 10);
    if((b == 0 && !is_zero(source[1])) || b > UINT32_MAX) return false;
    a = (uint32_t) b;
    target->player_no = a;
    b = strtoul(source[2], NULL, 10);
    if((b == 0 && !is_zero(source[2])) || b > UINT32_MAX) return false;
    a = (uint32_t) b;
    target->x_co = a;
    b = strtoul(source[3], NULL, 10);
    if((b == 0 && !is_zero(source[3])) || b > UINT32_MAX) return false;
    a = (uint32_t) b;
    target->y_co = a;
    return true;
}

/** Saves into the struct storing a batch-mode command the golden-move command.
 * @param[in, out] target - pointer to the target structure,
 * @param[in] source - pointer to the array of strings determining the command parameters.
 * @return true, if the command has been saved correctly, and false otherwise.
 */
static bool write_golden(game_command* target, char** source)
{
    bool res = write_gmove(target, source);
    target->type = golden;
    return res;
}

/** Saves into the struct storing a batch-mode command the command of calling
 *  the function free_fields, busy_fields or golden_possible, depending on the parameter com.
 * @param[in, out] target - pointer to the target structure,
 * @param[in] source - pointer to the array of strings determining the command parameters.
 * @param[in] com - enum determining the type of the called function.
 * @return true, if the command has been saved correctly, and false otherwise.
 */
static bool write_player_com(game_command* target, char** source, enum command_type com)
{
    if(source[1] == NULL || source[2] != NULL) return false;
    if(!string_is_digit(source[1])) return false;
    uint32_t a = strtoul(source[1], NULL, 10);
    if(a == 0 && !is_zero(source[1])) return false;
    target->type = com;
    target->player_no = a;
    return true;
}

/** Saves into the struct storing a batch-mode command the command of calling
 *  the function gamma_board.
 * @param[in, out] target - pointer to the target structure,
 * @param[in] source - pointer to the array of strings determining the command parameters.
 * @return true, if the command has been saved correctly, and false otherwise.
 */
static bool write_board(game_command* target, char** source)
{
    if(source[1] != NULL) return false;
    target->type = board;
    return true;
}

/** Saves into the struct storing a batch-mode command one of the correct commands.
 * @param[in, out] target - pointer to the target structure,
 * @param[in] source - pointer to the array of strings determining the command parameters.
 * @return true, if the command has been saved correctly, and false otherwise.
 */
static bool write_command(game_command* target, char** source)
{
    if(source == NULL || source[0] == NULL) return false;
    if(!strcmp(source[0], "m")) return write_gmove(target, source);
    if(!strcmp(source[0], "g")) return write_golden(target, source);
    if(!strcmp(source[0], "b")) return write_player_com(target, source, busy);
    if(!strcmp(source[0], "f")) return write_player_com(target, source, freef);
    if(!strcmp(source[0], "q")) return write_player_com(target, source, possible);
    if(!strcmp(source[0], "p")) return write_board(target, source);
    return false;
}

/** If a correct command can be obtained from the input, its parameters are saved into
 * the given struct storing a batch-mode command and true is returned. Otherise,
 * false is returned. Error messages are printed to the screen.
 * @param[in, out] target - pointer to the struct storing the command,
 * @param[in, out] line_no - pointer to the variable determining the input row number,
 * @param[in, out] mem_err - boolean variable, which is set to true if a memory error occurs.
 * @return true, if the command has been obtained and saved successfully, and false otherwise.
 */
static bool get_valid_command(game_command* target, int* line_no, bool* mem_err)
{
    bool loc_mem_err = false;
    bool is_ignored = false;
    bool has_eof = false;
    bool white_start = false;
    char** parsed = parse(&is_ignored, &has_eof, &white_start, &loc_mem_err);
    if(loc_mem_err)
    {
        *mem_err = true;
        return false;
    }
    (*line_no)++;
    while(!has_eof && ( !write_command(target, parsed) || is_ignored || white_start))
    {
        parsing_failure(line_no, &parsed, &is_ignored, &has_eof, &white_start, &loc_mem_err);
    }
    free_parsed(parsed);
    if(has_eof)
    {
        if(!is_ignored) print_error(*line_no);
        return false;;
    }
    else return true;
}

/** Executes a game command in the batch mode.
 * @param[in] c - pointer to the struct storing the command,
 * @param[in, out] g - pointer to the struct storing the game state,
 * @param[in] line_no - current row number.
 */
static void execute_command(game_command* c, gamma_t* g, int line_no)
{
    switch(c->type)
    {
        case gmove:
            printf("%d\n", gamma_move(g, c->player_no, c->x_co, c->y_co));
            break;
            
        case golden:
            printf("%d\n", gamma_golden_move(g, c->player_no, c->x_co, c->y_co));
            break;
            
        case busy:
            printf("%" PRIu64 "\n", gamma_busy_fields(g, c->player_no));
            break;
            
        case freef:
            printf("%" PRIu64 "\n", gamma_free_fields(g, c->player_no));
            break;
            
        case possible:
            printf("%d\n", gamma_golden_possible(g, c->player_no));
            break;
            
        case board:
            ; // C just won't have it otherwise :)
            char* image = gamma_board(g);
            if(image == NULL)
            {
                print_error(line_no);
                return;
            }
            else
            {
                printf(image);
                free(image);
            }
            break;
            
        default:
            break;
    }
}

void play_batch(gamma_t* g, int* line_no, bool* mem_err)
{
    bool loc_mem_err = false;
    game_command* c = malloc(sizeof(game_command));
    if(c == NULL)
    {
        *mem_err = true;
        return;
    }
    while(get_valid_command(c, line_no, &loc_mem_err))
    {
        if(loc_mem_err)
        {
            *mem_err = true;
            free(c);
            return;
        }
        execute_command(c, g, *line_no);
        if(loc_mem_err)
        {
            *mem_err = true;
            free(c);
            return;
        }
    }
    free(c);
    return;
}

