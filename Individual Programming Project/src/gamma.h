/** @file
 * IInterface of the class storing the game state.
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 18.03.2020
 */

#ifndef GAMMA_H
#define GAMMA_H

#include "auxiliary_structs.h"

#include <stdbool.h>
#include <stdint.h>

/** Checks, if any of the fields adjacent to ( @p x, @p y) belong to the player no. @p player.
 * @param[in] g   - pointer to the struct storing the game state,
 * @param[in] x       - the column number, non-negative integer smaller than the value of
 *                      @p width from the function @ref gamma_new,
 * @param[in] y       - the row number, non-negative integer smaller than the value of
 *                      @p height from the function @ref gamma_new,
 * @param[in] player  - player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new.
 * @return True, is such a field exists, and false otherwise.
 */
bool adjacent_owned_by_player(gamma_t *g, uint32_t x, uint32_t y, uint32_t player);

/** Determines the decimal representation length of x.
 * @param[in] x - non-negative integer.
 * @return The devimal representation length of @p x.
 */
unsigned int decimal_length(uint32_t x);

/** Struct storing the game state.
 */
typedef struct gamma gamma_t;

/** @brief Creates a structure storing the game state.
 * @param[in] width   – board width, positive integer,
 * @param[in] height  – board height, positive integer,
 * @param[in] players – number of players, positive integer,
 * @param[in] areas   – maximal number of areas of a given player,
 *                      a positive integer
 * @return A pointer to the created struct or NULL, if the creation failed
 *         or one of the parameters is invalid.
 */
gamma_t* gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas);

/** @brief Deletes the struct storing the game state.
 * @param[in] g       – pointer to the deleted struct.
 */
void gamma_delete(gamma_t *g);

/** @brief Executes a move.
 * Sets a checker of the @p player player on (@p x, @p y).
 * @param[in,out] g   – pointer to the struct storing the game state,
 * @param[in] player  – player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new,
 * @param[in] x       – the column number, non-negative integer smaller than the value of
 *                      @p width from the function @ref gamma_new,
 * @param[in] y       – the row number, non-negative integer smaller than the value of
 *                      @p height from the function @ref gamma_new.
 * @return @p true, if move has been executed, and @p false,
 * if it is illegal or one of the parameters is invalid.
 */
bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** Checks, if the given player can correctly execute a golden move on a given field.
 * @param[in] g – pointer to the struct storing the game state,
 * @param[in] new_owner - pointer to the struct storing the player,
 * @param[in] new_owner_num - number of this player, positive integer,
 * @param[in] x - the column number, non-negative integer smaller than the value of
 *                @p width from the function @ref gamma_new,
 * @param[in] y – the row number, non-negative integer smaller than the value of
 *                @p height from the function @ref gamma_new.
 * */
bool golden_possible_on_field(gamma_t* g, player_t* new_owner, uint32_t new_owner_num, uint32_t x, uint32_t y);

/** @brief Executes the golden move.
 * @param[in,out] g   – pointer to the struct storing the game state,
 * @param[in] player  – player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new,
 * @param[in] x       – the column number, non-negative integer smaller than the value of
 *                      @p width from the function @ref gamma_new,
 * @param[in] y       – the row number, non-negative integer smaller than the value of
 *                      @p height from the function @ref gamma_new.
 * @return @p true, if the move has been executed, and @p false,
 * if the player has already performed their golden move, the move is illegal, or one of
 * the parameters is invalid.
 */
bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Returns the number of fields occupied by a given player.
 * @param[in] g       – pointer to the struct storing the game state,
 * @param[in] player  – player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new.
 * @return The number of fields occupied by a player or zero, if one of the parameters
 *         is illegal.
 */
uint64_t gamma_busy_fields(gamma_t *g, uint32_t player);

/** @brief Returns the number of field which may be claimed by a given player in the next move.

 * @param[in] g       – pointer to the struct storing the game state,
 * @param[in] player  – player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new.
 * @return The number of field which may be claimed by a given player in the next move,
 * or zero, if one of the parameters is invalid.
 */
uint64_t gamma_free_fields(gamma_t *g, uint32_t player);

/** @brief Checks, if the player can execute their golden move.
 * @param[in] g       – pointer to the struct storing the game state,
 * @param[in] player  – player number, positive integer not bigger than the value of
 *                      @p players from the function @ref gamma_new.
 * @return @p true, if the player has not yet executed their golden move and there exists at least
 * one field occupied by a different player, and @p false otherise.
 */
bool gamma_golden_possible(gamma_t *g, uint32_t player);

/** @brief Returns a string storing the board state.
 * @param[in] g       - pointer to the struct storing the game state.
 * @return Pointer to the allocated buffer containing the string describing the board state,
 * or NULL, if memory allocation failed.
 */
char* gamma_board(gamma_t *g);

#endif //* GAMMA_H
