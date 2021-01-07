/** @file
 * Implementation of the functions responsible for managing input date as well as
 * two simple functions managing the output of the batch mode. 
 * wsadowym.
 *
 */
 

#include "parsing.h"

/** Reads a character from the input.
* @return The read character, or 0, if the program has reached the end of input data.
*/
static char safe_getchar()
{
    int c = getchar();
    if(c == EOF) return 0;
    return c;
}

/** Checks if c is a newline or EOF. 
 * @param[in] c - Checked character.
 * @return true, if c is a newline or EOF, and false otherwise.
 */
static bool can_end_line(char c)
{
    return (c == 0 || c == '\n');
}

/** Appends a character to a string. If necessary, increases the size of the string two times.
 * @param[in, out] target - pointer to the modified string,
 * @param[in] new - the appended character,
 * @param[in, out] length - pointer to an integer informing about the maximum number of characters
                            in the block allocated for the string,
 * @param[in, out] occupied - pointer to the variable informing about the number of already read characters.
 * @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 */
static void write_to(char** target, char new, unsigned int* length, unsigned int* occupied, bool* mem_err)
{
    if(*length == *occupied)
    {
        (*length) *= 2;
        char* temp = realloc(*target, (*length) * sizeof(char));
        if(temp != NULL) *target = temp;
        else
        {
            *mem_err = true;
            return;
        }
    }
    (*target)[*occupied] = new;
    (*occupied)++;
}

/** Trims the block of memory allocated for the string to a given length.
 * @param[in, out] target - pointer to string,
 * @param[in, out] length - the length of the block - number of characters.
 * @param[in, out] new_length - the desired length,
 * @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 * @return pointer to the resulting block.
 */
static char* trim(char* target, unsigned int length, unsigned int new_length, bool* mem_err)
{
    if(length!= new_length)
    {
        char* temp = NULL;
        temp = realloc(target, new_length*sizeof(char));
        if(temp == NULL)
        {
            free(target);
            *mem_err = true;
            return NULL;
        }
        else return temp;
    }
    return target;
}

/** Initializes reading a line.
 * @param[in] has_eof - boolean variable, changed to true, if the first character of the line being read 
 *                      is EOF.
 * @param[in] result_length - pointer to an integer, determining the length of the line being read,
 * @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 * @return pointer to the allocated start of the string storing the line being read.
 */
static char* init_line_reading(bool* has_eof, unsigned int* result_length, bool* mem_err)
{
    char c = safe_getchar();
    if(can_end_line(c))
    {
        if(c == 0) *has_eof = true;
        *result_length = 0;
        return NULL;
    }
    char* res = malloc(sizeof(char));
    if(res == NULL)
    {
        *mem_err = true;
        return NULL;
    }
    res[0] = c;
    return res;
}

/** Reads a whole line.
 * @param[in, out] has_eof - boolean variable, changed to true if EOF has been encountered,
 * @param[in, out] result_length - pointer to an integer, determining
 *                            the length of the line being read,
 * @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 * @return pointer to the allocated string storing the line being read.
 */
static char* get_whole_line(bool* has_eof, unsigned int* result_length, bool* mem_err)
{
    bool loc_mem_err = false;
    unsigned int length = 1;
    unsigned int occupied = 1;
    char* res = init_line_reading(has_eof, result_length, &loc_mem_err);
    if(loc_mem_err)
    {
        *mem_err = true;
        return NULL;
    }
    if(res == NULL) return NULL;
    char c = safe_getchar();
    while(!can_end_line(c))
    {
        write_to(&res, c, &length, &occupied, &loc_mem_err);
        if(loc_mem_err)
        {
            free(res);
            *mem_err = true;
            return NULL;
        }
        c = safe_getchar();
    }
    res = trim(res, length, occupied, &loc_mem_err);
    if(loc_mem_err) *mem_err = true;
    *has_eof = (c == 0);
    *result_length = occupied;
    return res;
}

/** Reads the next character from a string and increases the variable determining the
 *  number of read characters.
 * @param[in] source - pointer to the string,
 * @param[in] read_chars - pointer to an integer
 *                         storing the number of already read signs.
 * @return The next read sign.
 */
static char next_char(char* source, int* read_chars)
{
    *read_chars += 1;
    return source[*read_chars - 1];
}

/** Reads, into the string representing a token, single characters from the string representing the whole line. 
 * @param[in, out] target - pointer to the target string,
 * @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 * @param[in, out] read_chars - pointer to zmienna informujaca o liczbie wczytanych
 *                         juz znakow,
 * @param[in] source_length - length of the source string,
 * @param[in] source - pointer to the source string.
 * @return pointer to the whole of the read token.
 */
static char* read_letters(char* target, bool* mem_err, int* read_chars, int source_length, char* source)
{
    unsigned int local_length = 1;
    unsigned int local_occupied = 1;
    bool reached_space = false;
    char c;
    while(*read_chars < source_length && !reached_space)
    {
        c = next_char(source, read_chars);
        if(!isspace(c))
        {
            write_to(&target, c, &local_length, &local_occupied, mem_err);
            if(*mem_err)
            {
                free(target);
                return NULL;
            }
        }
        else reached_space = true;;
    }
    target = trim(target, local_length, local_occupied + 1, mem_err);
    if(*mem_err)
    {
        if(target != NULL) free(target);
        return NULL;
    }
    target[local_occupied] = '\0';
    return target;
}

/** Reads a single token from a string containing the contents of a single line. 
 *  Returns NULL if it has reached the end of the string.
 * @param[in] source - pointer to the source string,
 * @param[in] source_length - length of the string in characters,
 * @param[in] read_chars - pointer to the variable informing about the number of already read characters,
 * @param[in, out] mem_err - pointer to a boolean variable, changed to true in case of a memory error.
 * @return pointer to string containing the read token or NULL, if the whole string has been read or
 *         a memory error occurred.
 */
static char* read_token(char* source, int source_length, int* read_chars, bool* mem_err)
{
    if(source_length == *read_chars) return NULL;
    bool loc_mem_err = false;
    char c = next_char(source, read_chars);
    while(*read_chars < source_length && isspace(c)) c = next_char(source, read_chars);
    if(isspace(c)) return NULL;
    char* res = malloc(sizeof(char));
    if(res == NULL)
    {
        *mem_err = true;
        return NULL;
    }
    res[0] = c;
    res = read_letters(res, &loc_mem_err, read_chars, source_length, source);
    if(loc_mem_err)
    {
        *mem_err = true;
        return NULL;
    }
    return res;
}

/** Checks if the string can be correctly converted to uint32_t.
 * @param[in] s - pointer to the checked string.
 * @return true, if the string can be correctly converted to uint32_t,
 *               and false otherwise.
 */
static bool can_make_uint32(char* s)
{
    // Program dopuszcza zera wiodace we wszystkich parametrach.
    if(!string_is_digit(s)) return false;
    unsigned long int x = strtoul(s, NULL, 10);
    if(x == 0 && !is_zero(s)) return false;
    if(x > UINT32_MAX) return false;
    return true;
}

/** Checks if a given array of string constitutes a valid command of game initialization. 
 * @param[in] target - pointer to the checked array.
 * @return true, if the string is a valid command of game initialization,
 *               and false otherwise.
 */
static bool is_valid_init(char** target)
{
    if(target == NULL) return false;
    if(target[5] != NULL || target[4] == NULL) return false;
    if(!(strcmp("I", target[0]) == 0 || strcmp("B", target[0]) == 0)) return false;
    for(int i = 1; i < 5; i++)
    {
        if(!can_make_uint32(target[i])) return false;
    }
    return true;
}

void free_parsed(char** target)
{
    if(target != NULL)
    {
        for(int i = 0; i < 6; i++) if(target[i] != NULL) free(target[i]);
        free(target);
    }
}

char** parse(bool* is_ignored, bool* has_eof, bool* white_start, bool* mem_err)
{
    bool loc_mem_err = false;
    unsigned int line_length = 0;
    char* line = get_whole_line(has_eof, &line_length, &loc_mem_err);
    for(unsigned int i = 0; i < line_length; i++)
    {
        if(line[i] == '\0')
        {
            free(line);
            return NULL;
        }
    }
    if(loc_mem_err)
    {
        *mem_err = true;
        return NULL;
    }

    if(line == NULL)
    {
        *is_ignored = true;
        return NULL;
    }
    *is_ignored = (line[0] == '#' || line[0] == '\n');
    if(*has_eof) *is_ignored = false;
    *white_start = isspace(line[0]);
    char** tokens = calloc(6, sizeof(char*));
    int read_chars = 0;
    for(int i = 0; i < 6; i++)
    {
        tokens[i] = read_token(line, line_length, &read_chars, &loc_mem_err);
        if(loc_mem_err)
        {
            *mem_err = true;
            free_parsed(tokens);
            return NULL;
        }
    }
    free(line);
    return tokens;
}

bool is_zero(char* s)
{
    unsigned int length = strlen(s);
    if(length == 0) return false;
    for(unsigned int i = 0; i < length; i++) if(s[i] != '0') return false;
    return true;
}

bool string_is_digit(char* s)
{
    int length = strlen(s);
    if(length == 0) return false;
    for(int i = 0; i < length; i++) if(!isdigit(s[i])) return false;
    return true;
}

void parsing_failure(int* line_no, char*** parsed, bool* is_ignored, bool* has_eof, bool* white_start, bool* mem_err)
{
    bool loc_mem_err = false;
    free_parsed(*parsed);
    if(!(*is_ignored)) print_error(*line_no);
    (*line_no)++;
    *parsed = parse(is_ignored, has_eof, white_start, &loc_mem_err);
    if(loc_mem_err) *mem_err = true;
}


char** get_valid_init(int* line_no, bool* mem_err)
{
    bool loc_mem_err = false;
    bool is_ignored = false;
    bool has_eof = false;
    bool white_start = false;
    char** parsed = parse(&is_ignored, &has_eof, &white_start, &loc_mem_err);
    if(loc_mem_err)
    {
        *mem_err = true;
        return NULL;
    }
    (*line_no)++;
    while(!has_eof && ( !is_valid_init(parsed) || is_ignored || white_start))
    {
        parsing_failure(line_no, &parsed, &is_ignored, &has_eof, &white_start, &loc_mem_err);
    }
    if(has_eof)
    {
        if(!is_ignored) print_error(*line_no);
        free_parsed(parsed);
        return NULL;
    }
    else return parsed;
}

gamma_t* game_from_parsed_line(char** line)
{
    uint32_t width = strtoul(line[1], NULL, 10);
    uint32_t height = strtoul(line[2], NULL, 10);
    uint32_t players = strtoul(line[3], NULL, 10);
    uint32_t areas = strtoul(line[4], NULL, 10);
    return gamma_new(width, height, players, areas);
}

void print_error(int line_number)
{
    fprintf(stderr, "ERROR %d\n", line_number);
}

void print_ok(int line_number)
{
    fprintf(stdout, "OK %d\n", line_number);
}
