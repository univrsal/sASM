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

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#ifdef WIN
#include <io.h>

#ifndef F_OK
#define F_OK 0x00
#endif
#elif UNIX
#include <unistd.h>
#endif

sasm_bool util_file_exists(const char* path)
{
    if (!path)
        return sasm_false;
    int result;
#ifdef WIN
    result = access(path, F_OK);
#elif UNIX
    result = access(path, F_OK);
#endif
    return result == -1 ? sasm_false : sasm_true;
}

sasm_bool util_file_empty(const char* path)
{
    if (!path)
        return sasm_true;
    FILE *fp = fopen(path, "r");

    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        long s = ftell(fp);
        if (s == 0)
        {
            fclose(fp);
            return sasm_true;
        }
        fclose(fp);
    }

    return sasm_false;
}

void util_cut_str_end(char* str, char c)
{
    if (strchr(str, c))
        str[strlen(str) - strlen(strrchr(str, c))] = '\0';
}

/* StackOverflow C&P */
char** util_str_split(char* str, const char delimiter, int* splits)
{
    char** result = 0;
    size_t count = 0;
    char* tmp = str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = delimiter;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (delimiter == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    *splits = count;
    /* Add space for trailing token. */
    count += last_comma < (str + strlen(str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result) {
        size_t idx = 0;
        char* token = strtok(str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

void util_free_strings(char** c)
{
    if (c) {
        int i;
        for (i = 0; *(c + i); i++)
            free(*(c + i));
        free(c);
        c = NULL;
    }
}

void util_replace_char(char* str, char orig, char repl)
{
    char* copy = str;
    while ((copy = strchr(copy, orig)) != NULL)
        *copy++ = repl;
}

void util_cut_str_begin(char** str, char c)
{
    char* result = strchr(*str, c);
    result++;
    *str = result;
}
