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

#ifndef SASM_UTIL_H
#define SASM_UTIL_H

typedef enum { sasm_false = 0, sasm_true } sasm_bool;

sasm_bool util_file_exists(const char* path);

sasm_bool util_file_empty(const char* path);

void util_cut_str_end(char* str, char c);

void util_cut_str_begin(char** str, char c);
#endif //SASM_UTIL_H
