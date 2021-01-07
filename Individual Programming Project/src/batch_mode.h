/** @file
 * Interface of the functions responsible for executing the batch mode.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include "auxiliary_structs.h"
#include "gamma.h"
#include "parsing.h"
 
#ifndef BATCH_MODE
#define BATCH_MODE


/** Executes the game in batch mode.
 * @param[in] g - pointer to the struct storing the game state,
 * @param[in] line_no - pointer to the variable determining the row number,
 * @param[in, out] mem_err - boolean variable, modified to true in case of
 *                           a memory error.
 */
void play_batch(gamma_t* g, int* line_no, bool* mem_err);


#endif // BATCH_MODE
