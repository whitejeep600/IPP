/** @file
 * Interface of the functions responsible for interactive game.
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
#include "auxiliary_structs.h"
#include "terminal_management.h"
#include "gamma.h"

#ifndef PLAY_INTERACTIVE
#define PLAY_INTERACTIVE

/** Executes an interactive game.
 * @param[in, out] g - pointer to the struct storing the game state,
 * @param[in, out] mem_err - pointer to a boolean type variable, changed to true
 *                           in case of a memory error.
 */
void play_interactive(gamma_t* g, bool* mem_err);

#endif // PLAY_INTERACTIVE
