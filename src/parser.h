/* Created by univrsal on 21/01/2019
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

#ifndef SASM_PARSER_H
#define SASM_PARSER_H

#include <stdarg.h>
#include <stdint.h>
#include "asm.h"

/* Parse assembly text files and write them to binary */
#define LABEL_MAX_LENGTH    16
struct sasm_asm;

typedef enum
{
    sasm_general_failure
} sasm_error_code;

typedef struct sasm_parse_error
{
    size_t line;
    sasm_error_code type;
} sasm_parse_error_t;

typedef struct sasm_parse_result
{
    size_t error_count;
    sasm_parse_error_t** errors;
    size_t addr_space;
} sasm_parse_result_t;

typedef struct sasm_label
{
    char id[LABEL_MAX_LENGTH];
    uint16_t address;
} sasm_label_t;

/* Result contains potential errors, has to be freed */
sasm_parse_result_t* sasm_build_asm(sasm_asm_t* sasm, const char* input, const char* output);

sasm_label_t** parse_labels(sasm_parse_result_t* result, sasm_asm_t* sasm, FILE* f, size_t* count);

void create_asm(sasm_parse_result_t* result, sasm_asm_t* sasm, FILE* f, sasm_label_t** labels);

#endif //SASM_PARSER_H