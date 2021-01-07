/** @file
 * Interface of the fucntions responsible for terminal management in interactive mode.
 */

#ifndef TERMINAL_MANAGEMENT
#define TERMINAL_MANAGEMENT


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include "auxiliary_structs.h"
#include "gamma.h"


/** Sets the terminal cursor na to given coefficients.
 * @param[in] x - x coefficient,
 * @param[in] y - y coefficient.
 */
void set_cursor_on_coeffs(uint32_t x, uint32_t y);

/** Sets the terminal cursor at the beginning of a given line.
 * @param[in] line_no - target line number, positive integer.
 */
void set_cursor_to_line(uint32_t line_no);

/** Sets the terminal cursor so that its position corresponds to the board field
 *  which the virtual cursor points to.
 * @param[in] cursor - pointer to the structure storing the program cursor.
*/
void set_cursor_on_board(cursor_t* cursor);

/** Moves the cursor to the left.
 * @param[in] n - number of cells by which the cursor is moved.
 */
void cursor_left(int n);

/** Reverses the color of background and characters in the terminal.
 */
void reverse_video(void);

/** Restores the default terminal text settings.
 */
void reset_video(void);

/** Clears the screen.
 */
void clear_screen(void);

/** Sets yellow as background color.
 */
void background_to_yellow(void);

/** Sets yellow as text color.
 */
void foreground_to_yellow(void);

/** Sets red as background color.
 */
void background_to_red(void);

/** Sets red as text color.
 */
void foreground_to_red(void);

/** Sets magenta as background color.
 */
void background_to_magenta(void);

/** Sets magenta as text color.
 */
void foreground_to_magenta(void);

/** Sets black as text color.
 */
void foreground_to_black(void);

/** Initializes the values of virtual terminal cursor for a given game.
 * @param[in] g - pointer to the struct storing game state,
 * @param[in, out] cursor - pointer to the struct storing
 *                          the virtual game cursor.
 */
void initialize_cursor(gamma_t* g, cursor_t* cursor);

/** Checks if the terminal size is sufficiently big to start the game in interactive state. S
 * @param[in] g -pointer to the struct storing the game state.
 * @return True, if the windows is sufficiently big,
 *         and false otherwise.
 */
bool correct_terminal(gamma_t* g);

/** Sets the terminal to reading characters without bufffering or echoing to screen.
 * @param[in, out] normal_settings - pointer to the termios struct which is to store
 *                                   the default values,
 * @param[in, out] unbuff - pointer to the termios struct which is to store
 *                          the settings for non-buffering and non-echoing mode.
 */
void set_unbuff_input(struct termios* normal_settings, struct termios* unbuff);

/** Sets the terminal back to default settings.
 * @param[in] normal_settings - pointer to the termios struct storing
 *                              the default terminal settings.
 */
void set_normal_input(struct termios* normal_settings);

#endif // TERMINAL_MANAGEMENT
