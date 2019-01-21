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

#define LINE_LENGTH  256

sasm_asm* sasm_asm_load(const char *file)
{
    sasm_bool can_load = util_file_exists(file) && !util_file_empty(file);

    if (!can_load)
        return NULL;
    FILE* f = fopen(file, "r");
    return sasm_asm_load_(f);
}

sasm_asm* sasm_asm_load_(FILE* f)
{
    char buf[LINE_LENGTH];
    char** splits = NULL;
    sasm_mnemonic* new_mnemonic = NULL;
    sasm_asm* sasm = malloc(sizeof(sasm_asm));
    sasm->mnemonic_count = 0;
    sasm->mnemonics = NULL;

    while (fgets(buf, LINE_LENGTH, f) != NULL)
    {
        if (strlen(buf) < 1 || buf[0] == ';')
            continue;

        new_mnemonic = malloc(sizeof(sasm_mnemonic));
        new_mnemonic->type = sasm_mnemonic_proc;

        util_replace_char(buf, '\n', '\0');
        util_cut_str_end(buf, ';'); /* Cut off any comments at the end */
        if (strlen(buf) < 1)
            continue;

        int count = 0;
        splits = util_str_split(buf, ' ', &count);

        if (count == 0) {
            util_free_strings(splits);
            continue;
        }

        if (count > 1) /* There's still text left */
        {
            if (strstr(splits[2], "ADDR"))
                new_mnemonic->type = sasm_mnemonic_jump;
            else if (strstr(splits[2], "INT"))
                new_mnemonic->type = sasm_mnemonic_fun_int;
            else
                new_mnemonic->type = sasm_mnemonic_fun;
            /* Count > 2 -> Warning? */
        }

        memcpy(new_mnemonic->id, splits[1], strlen(splits[1]) + 1);
        new_mnemonic->op_code = (uint8_t) strtol(splits[0], NULL, 16);
        sasm->mnemonics = realloc(sasm->mnemonics, (sasm->mnemonic_count + 1) * sizeof(sasm_mnemonic*));
        sasm->mnemonics[sasm->mnemonic_count] = new_mnemonic;
        sasm->mnemonic_count++;
        util_free_strings(splits);
    }

    for (int i = 0; i < sasm->mnemonic_count; i++)
        printf("0x%X|%i|%s|\n", sasm->mnemonics[i]->op_code,
               sasm->mnemonics[i]->type, sasm->mnemonics[i]->id);
    return sasm;
}
