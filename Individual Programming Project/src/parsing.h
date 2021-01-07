/** @file
 * Interface of the functions responsible for managing input date as well as
 * two simple functions managing the output of the batch mode.
 */
 
 #ifndef PARSING
 #define PARSING
 
 #include <stdbool.h>
 #include <ctype.h>
 #include <string.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include "gamma.h"
 #include "auxiliary_structs.h"


/** Frees the six-element string array representing the line being read.
 * @param[in] target - pointer to the array to free.
 */
void free_parsed(char** target);

/** Reads a line from input and divides it into tokens.
 * @param[in, out] is_ignored - boolean variable, changed to true,
 *                              if the line is ignored,
 * @param[in, out] has_eof - boolean variable, changed to true, if 
 *                           the line contains EOF,
 * @param[in, out] white_start - boolean variable, changed to true, if 
 *                           the line begins with a white character,
* @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 @return pointer to the allocated array of strings or NULL in case of a memory error
         or a line ignored by the program.
 */
char** parse(bool* is_ignored, bool* has_eof, bool* white_start, bool* mem_err);

/** Checks if a string only contains zeroes.
 * @param[in] s - pointer to the checked string.
 * @return true, if the string only contains zeroes, and false otherwise.
 */
bool is_zero(char* s);

/** Checks if the string only contains numbers.
 * @param[in] s - pointer to the checked string.
 * @return true, if the string only contains numbers, and false otherwise.
 */
bool string_is_digit(char* s);

/** Manages the situation in which a line is not a correct command. Prints an error message if necessary.
 * Reads the next line.
 * @param[in, out] line_no - pointer to the variable storing the line number,
 * @param[in, out] parsed - pointer to pointer to an array of strings containing
 *                          the representation of the previous line, an supposed to contain that of the
 *                          current one after the function call,
 * @param[in, out] is_ignored - pointer to a boolean variable informing if the line is 
 *                              ignored by the program,
 * @param[in, out] has_eof - pointer to a boolean variable informing if the line contains EOF,
 * @param[in, out] white_start - pointer to a boolean variable informing if the line
 *                               begins with a white character,
* @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 */
void parsing_failure(int* line_no, char*** parsed, bool* is_ignored, bool* has_eof, bool* white_start, bool* mem_err);

/** Reads the first valid command of game initialization.
 * @param[in, out] line_no - pointer to the variable storing the current row number,
* @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 * @return A string array representing the first valid game initialization command, or NULL, if there is no such command.
 */
char** get_valid_init(int* line_no, bool* mem_err);

/** Creates a struct containing the game state based on an array of strings, storing the respective parameters of the
 * @ref gamma_new function.
 * @param[in] line - pointer to the source string array.
 * @return - pointer to the allocated structure or NULL, if
 *           memory allocation failed.
 */
gamma_t* game_from_parsed_line(char** line);

/** Prints an error message for a given line number.
 * @param[in] line_number - the line number.
 */
void print_error(int line_number);

/** Prints a message about a successful game initialization for a given line number.
 * @param[in] line_number - the line number.
 */
void print_ok(int line_number);
 
#endif /// PARSING
