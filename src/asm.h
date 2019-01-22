/* Created by univrsal on 20/01/2019
 * This file is part of sasm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * github.com/univrsal/sasm
 */

#ifndef SASM_ASM_H
#define SASM_ASM_H
/* Reads mnemonics from plain text file for parsing */

#include <stdint.h>
#include <stdio.h>

#define MNEMONIC_MAX    6
#define ARG_MAX         8

typedef enum
{
    sasm_false = 0, sasm_true
} sasm_bool;

/* Mnemonics can either be
 *  - a single instruction
 *  - an instruction with a secondary argument (e.g. MOV A,B)
 *  - an instruction with a number as a secondary argument
 *  - an instruction with a label or address as a secondary argument
 */
typedef enum
{
    sasm_mnemonic_invalid = -1,
    sasm_mnemonic_op,       /* Operation                */
    sasm_mnemonic_fun,      /* Function with argument   */
    sasm_mnemonic_fun_int,  /* Argument is an integer   */
    sasm_mnemonic_jump,     /* Jump/call with address   */
    sasm_mnemonic_count
} sasm_mnemonic_type;

typedef struct sasm_mnemonic
{
    char id[MNEMONIC_MAX];  /* eg. 'MOV' */
    char arg[ARG_MAX];      /* eg. the 'A' in 'PUSH A' */
    sasm_mnemonic_type type;
    uint8_t op_code;
} sasm_mnemonic_t;

/* Contains all mnemonics for a loaded assembly language */
typedef struct sasm_asm
{
    size_t mnemonic_count;
    sasm_mnemonic_t** mnemonics;
    sasm_bool debug;
} sasm_asm_t;

/* Checks if file exists and isn't empty */
sasm_asm_t* sasm_asm_load(const char* file);
/* Doesn't perform any checks */
sasm_asm_t* sasm_asm_load_(FILE* f);

void sasm_asm_free(sasm_asm_t* sasm);

/* Prints out all mnemonics to verify loading worked */
void sasm_print_asm(sasm_asm_t* sasm);

sasm_mnemonic_t* sasm_parse_line(sasm_asm_t* sasm, const char* line);

sasm_mnemonic_type sasm_parse_type(sasm_asm_t* sasm, const char* line);

#endif //SASM_ASM_H
