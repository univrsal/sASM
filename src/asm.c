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

#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "asm.h"
#include "util.h"
#include "parser.h"

sasm_asm_t* sasm_asm_load(const char* file)
{
    if (!util_valid_file(file))
    {
        printf("Error: %s can not be opened.\n", file);
        return NULL;
    }
    FILE* f = fopen(file, "r");
    return sasm_asm_load_(f);
}

sasm_asm_t* sasm_asm_load_(FILE* f)
{
    char buf[LINE_LENGTH];
    char** splits = NULL;
    sasm_mnemonic_t* new_mnemonic = NULL;
    sasm_asm_t* sasm = malloc(sizeof(sasm_asm_t));

    sasm->mnemonic_count = 0;
    sasm->mnemonics = NULL;

    while (fgets(buf, LINE_LENGTH, f) != NULL) {
        util_replace_char(buf, ';', '\0'); /* strip unwanted parts */
        util_trim_str(buf);

        if (strlen(buf) < 1) /* skip empty or comment lines */
            continue;
        
        int count = 0;
        splits = util_str_split(buf, ' ', &count);

        /* Line only contained one word. Format expects two */
        if (count < 1) {
            util_free_strings(splits);
            continue;
        }

        new_mnemonic = malloc(sizeof(sasm_mnemonic_t));
        new_mnemonic->type = sasm_mnemonic_op;
        new_mnemonic->arg[0] = '-';
        new_mnemonic->arg[1] = '\0';

        /* If there's a string after the mnemonic name  */
        if (count > 1) {
            if (strstr(splits[2], "ADDR")) {
                new_mnemonic->type = sasm_mnemonic_jump;
            }
            else if (strstr(splits[2], "INT")) {
                new_mnemonic->type = sasm_mnemonic_fun_int;
            }
            else {
                new_mnemonic->type = sasm_mnemonic_fun;
                memcpy(new_mnemonic->arg, splits[2], strlen(splits[2]) + 1);
            }
            /* Count > 2 -> Warning? */
        }

        memcpy(new_mnemonic->id, splits[1], strlen(splits[1]) + 1);
        new_mnemonic->op_code = (uint8_t) strtol(splits[0], NULL, 16);
        sasm->mnemonics = realloc(sasm->mnemonics, (sasm->mnemonic_count + 1) * sizeof(sasm_mnemonic_t*));
        sasm->mnemonics[sasm->mnemonic_count] = new_mnemonic;
        sasm->mnemonic_count++;

        util_free_strings(splits);
    }

    return sasm;
}

void sasm_asm_free(sasm_asm_t* sasm)
{
    if (sasm && sasm->mnemonic_count > 0) {
        int i = 0;
        while (i < sasm->mnemonic_count)
            free(sasm->mnemonics[i++]);

        free(sasm->mnemonics);
    }
}

void sasm_print_asm(sasm_asm_t* sasm)
{
    if (sasm) {
        printf("+--- %02llu Mnemonics loaded --\\\n", sasm->mnemonic_count);
        printf("| OP Code | Type | ID | Argument\n");
        for (int i = 0; i < sasm->mnemonic_count; i++) {
            printf("| 0x%02X | %i | %-6s | %-8s\n", sasm->mnemonics[i]->op_code,
                   sasm->mnemonics[i]->type, sasm->mnemonics[i]->id,
                   sasm->mnemonics[i]->arg);
        }
        if (!sasm->debug)
            printf("+-----------------/\n");
    }
}

sasm_mnemonic_t* sasm_parse_line(sasm_asm_t* sasm, const char* line, char*** splits)
{
    if (!sasm || !line)
        return NULL;

    sasm_mnemonic_t* result = NULL;
    int i, j, count = 0;
    sasm_bool found = sasm_false;
    *splits = util_str_split(line, ' ', &count);

    for (i = 0; i < sasm->mnemonic_count; i++) {

        if (!strcmp(sasm->mnemonics[i]->id, (*splits)[0])) {
            switch (sasm->mnemonics[i]->type) {
                case sasm_mnemonic_op: /* No arguments & id matches */
                    found = sasm_true;
                    break;
                case sasm_mnemonic_fun: /* Argument has to match */
                    if (count >= 1 && !strcmp(sasm->mnemonics[i]->arg, (*splits)[1]))
                        found = sasm_true;
                    break;
                case sasm_mnemonic_fun_int: /* Argument has to be an int */
                    if (count >= 1 && util_valid_int((*splits)[1]))
                        found = sasm_true;
                    break;
                case sasm_mnemonic_jump:
                    if (count >= 1) {
                        /* Check that the jump argument, isn't an argument for
                         * another mnemonic. e.g. JMP A is different from JMP 0x0e
                         * 'A' is an argument for a different JMP operation, while
                         * 0x0e, or any other label is for the regular jmp operation */

                        found = sasm_true; /* assume true, for loop will change it otherwise */
                        for (j = 0; j < sasm->mnemonic_count; j++) {
                            if (!strcmp((*splits)[1], sasm->mnemonics[j]->arg)) {
                                found = sasm_false;
                                break;
                            }
                        }
                    }

                    break;
                default:;
            }
        }

        if (found) {
            result = sasm->mnemonics[i];
            break;
        }
    }

    return result;
}

sasm_mnemonic_type sasm_parse_type(sasm_asm_t* sasm, const char* line)
{
    char** splits = NULL;
    sasm_mnemonic_t* parsed = sasm_parse_line(sasm, line, &splits);
    util_free_strings(splits);

    if (parsed)
        return parsed->type;
    return sasm_mnemonic_invalid;
}
