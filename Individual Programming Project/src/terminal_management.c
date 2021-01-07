/** @file
 * Implementation of the functions responsible for managing the terminal in interactive mode.
 */

#include "terminal_management.h"


/** Hides the terminal cursor.
 */
static void hide_cursor()
{
    printf("\x1b[?25l");
}

/** Shows the terminal cursor.
 */
static void show_cursor()
{
    printf("\x1b[?25h");
}

void set_cursor_on_coeffs(uint32_t x, uint32_t y)
{
    printf("\x1b[%" PRIu32 ";%" PRIu32 "H", y, x);
}

void set_cursor_to_line(uint32_t line_no)
{
    printf("\x1b[%" PRIu32";1H", line_no);
}

void set_cursor_on_board(cursor_t* cursor)
{
    uint32_t cursor_x = (cursor->x + 1) * cursor->field_width;
    if(cursor->field_width != 1) cursor_x -= 1;
    // Integer overflow is not a problem due to natural limitations on
	// the terminal window size.
    uint32_t cursor_y = cursor->height - cursor->y;
    set_cursor_on_coeffs(cursor_x, cursor_y);
}

void cursor_left(int n)
{
    printf("\x1b[%dD", n);
}

void reverse_video()
{
    printf("\x1b[7m");
}

void reset_video()
{
    printf("\x1b[0m");
}

void clear_screen()
{
    printf("\x1b[H\x1b[J");
}

void background_to_yellow()
{
    printf("\x1b[43m");
}

void foreground_to_yellow()
{
    printf("\x1b[33m");
}

void background_to_red()
{
    printf("\x1b[41m");
}

void foreground_to_red()
{
    printf("\x1b[31m");
}

void background_to_magenta()
{
    printf("\x1b[45m");
}

void foreground_to_magenta()
{
    printf("\x1b[35m");
}

void foreground_to_black()
{
    printf("\x1b[30m");
}

void initialize_cursor(gamma_t* g, cursor_t* cursor)
{
    cursor->x = 0;
    cursor->y = g->height_y - 1;
    cursor->height = g->height_y;
    unsigned int aux_field = decimal_length(g->n_of_players);
    if(aux_field == 1) cursor->field_width = 1;
    else cursor->field_width = aux_field + 1;
}

bool correct_terminal(gamma_t* g)
{
    struct winsize terminal;
    ioctl(0, TIOCGWINSZ, &terminal);
    unsigned int dec_length = decimal_length(g->n_of_players);
    if(dec_length == 1)
    {
        if(terminal.ws_col < g->width_x) return false;
    }
    else
    {
        if(terminal.ws_col < g->width_x * (dec_length + 1)) return false;
    }
    if(terminal.ws_row < g->height_y + 6) return false;
    // ^ 6 additional lines are included here for a text prompt.
    return true;
}

void set_unbuff_input(struct termios* normal_settings, struct termios* unbuff)
{
    tcgetattr(STDIN_FILENO, normal_settings);
    tcgetattr(STDIN_FILENO, unbuff);
    unbuff->c_lflag &= ~ICANON;
    unbuff->c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, unbuff);
    hide_cursor();
}

void set_normal_input(struct termios* normal_settings)
{
    tcsetattr(STDIN_FILENO, TCSANOW, normal_settings);
    show_cursor();
}
