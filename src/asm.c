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

    char* op_code = malloc(sizeof(char) * 2);
    char* mnemonic = malloc(sizeof(char) * MNEMONIC_MAX);
    char* argument = malloc(sizeof(char) * 16);

    while (fgets(buf, LINE_LENGTH, f) != NULL)
    {
        if (strlen(buf) < 1 || buf[0] == ';')
            continue;
        memccpy(op_code, buf, ' ', 2);
        
        mnemonic = buf + 3;

        mnemonic[strchr(mnemonic, ' ') - mnemonic] = '\0';
    }

    free(op_code);
    free(mnemonic);
    free(argument);
    return NULL;
}
