/** @file
 * Main file responsible for executing the game.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdint.h>
#include "gamma.h"
#include "auxiliary_structs.h"
#include "batch_mode.h"
#include "interactive_mode.h"
#include "parsing.h"

/** The main function executing the program.
 */
int main()
{
    bool mem_err = false;
    int line_no = 0;
    char** game_init;
    gamma_t* g = NULL;
    do
    {
        game_init = get_valid_init(&line_no, &mem_err);
        if(mem_err)
        {
            free_parsed(game_init);
            exit(1);
        }
        if(game_init == NULL)
        {
            free_parsed(game_init);
            return 0;
        }
        g = game_from_parsed_line(game_init); 
        if(g == NULL)
        {
            free_parsed(game_init);
            print_error(line_no);
        }
    }
    while(g == NULL);
    print_ok(line_no);
    if(strcmp("B", game_init[0]) == 0)
    {
        play_batch(g, &line_no, &mem_err);
        if(mem_err)
        {
            free_parsed(game_init);
            gamma_delete(g);
            return 1;
        }
    }
    if(strcmp("I", game_init[0]) == 0)
    {
        play_interactive(g, &mem_err);
    }
    gamma_delete(g);
    free_parsed(game_init);
    if(mem_err) return 1;
    return 0;
}
