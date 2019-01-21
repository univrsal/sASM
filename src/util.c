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

#include "util.h"
#ifdef WIN
#include <io.h>
#include <Windows.h>
#include <stdio.h>

#define F_OK 0x00

#endif

sasm_bool util_file_exists(const char* path)
{
    if (!path)
        return sasm_false;
    int result;
#ifdef WIN
    result = access(path, F_OK);
#elif UNIX
    result = access(file_path, F_OK); /* TODO: unix wstring? */
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
    str[strlen(str) - strlen(strrchr(str, c))] = '\0';
}

void util_cut_str_begin(char** str, char c)
{
    char* result = strchr(*str, c);
    result++;
    *str = result;
}
