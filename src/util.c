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
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "util.h"
#include "parser.h"

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
char** util_str_split(const char* str, const char delimiter, int* splits)
{
    char** result = 0;
    size_t count = 0;
    char* tmp = (char*) str;
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

    *splits = (int) count;
    /* Add space for trailing token. */
    count += last_comma < (str + strlen(str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result) {
        size_t idx = 0;
        char* token = strtok((char*) str, delim);

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
    }
}

void util_replace_char(char* str, char orig, char repl)
{
    char* copy = str;
    while ((copy = strchr(copy, orig)) != NULL)
        *copy++ = repl;
}

sasm_bool util_valid_file(const char* path)
{
    return util_file_exists(path) && !util_file_empty(path);
}

sasm_bool util_create_file(const char* path)
{
    FILE* f = fopen(path, "ab+");
    sasm_bool result = f ? sasm_true : sasm_false;
    if (result) fclose(f);
    return result;
}

sasm_bool util_valid_int(const char* str)
{
    sasm_bool result = sasm_false;
    if (str) {
        if (strstr(str, "0x") != NULL) /* Hex */
        {
            if (util_valid_hex(str + 2)) /* 0x excluded */
                result = sasm_true;
        } else if (strstr(str, "0b") != NULL) /* Binary */
        {
            if (util_valid_binary(str + 2)) /* 0b excluded */
                result = sasm_true;
        } else /* Assume int */
        {
            if (util_valid_decimal(str)) /* 0b excluded */
                result = sasm_true;
        }
    }
    return result;
}

sasm_bool util_valid_hex(const char* str)
{
    return str[strspn(str, "0123456789abcdefABCDEF")] == 0;
}

sasm_bool util_valid_binary(const char* str)
{
    return str[strspn(str, "01")] == 0 &&
           str[0] == '0' && str[1] == 'b';
}

sasm_bool util_valid_decimal(const char* str)
{
    return str[strspn(str, "0123456789")] == 0;
}

sasm_bool util_valid_mnemonic(sasm_mnemonic_type t)
{
    return t > sasm_mnemonic_invalid && t < sasm_mnemonic_count;
}

sasm_bool util_valid_label(const char* str)
{
    sasm_bool valid_characters = str[strspn(str, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_:")] == 0;
    sasm_bool colon_at_end = str[strlen(str) - 1] == ':';

    int colon_count = 0;
    char* copy = (char*) str;
    for (colon_count = 0; copy[colon_count];
         copy[colon_count] == ':' ? colon_count++ : *copy++);

    return valid_characters && colon_at_end && colon_count == 1
           && strlen(str) < LABEL_MAX_LENGTH;
}

void util_trim_str(char* str)
{
    for (size_t i = strlen(str) - 1; i > 0; i--) {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t'
            || str[i] == ';')
            str[i] = '\0';
        else
            break;
    }
}

void util_cut_str_begin(char** str, char c)
{
    char* result = strchr(*str, c);
    result++;
    *str = result;
}

sasm_bool util_parse_int(const char* str, uint8_t* result)
{
    if (util_valid_binary(str + 2))
    {
        *result = strtol(str + 2, NULL, 2);
        return sasm_true;
    }
    else
    {
        if (util_valid_hex(str + 2))
        {
            *result = strtol(str, NULL, 16);
            return sasm_true;
        }
        else
        {
            *result = strtol(str, NULL, 10);
            return sasm_true;
        }
    }
    return sasm_false;
}