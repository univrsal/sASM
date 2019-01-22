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

#include <malloc.h>
#include <string.h>
#include "util.h"
#include "parser.h"
#include "asm.h"

sasm_parse_result_t* sasm_build_asm(sasm_asm_t* sasm, const char* input, const char* output)
{
    if (!sasm || !input || !output)
        return NULL;

    if (!util_valid_file(input)) {
        printf("Error: %s doesn't exist.\n", input);
        return NULL;
    }

    if (!util_create_file(output)) {
        printf("Error: %s couldn't be created.\n", output);
        return NULL;
    }

    FILE* input_file = fopen(input, "r");
    if (!input_file) {
        printf("Error opening file\n");
        return NULL;
    }

    sasm_parse_result_t* result = malloc(sizeof(sasm_parse_result_t));
    sasm_label_t** labels = NULL;

    /* First collect all labels in the file to allow CALLs and JMPs */
    size_t label_count;
    labels = parse_labels(result, sasm, input_file, &label_count);

    if (sasm->debug) {
        printf("=== Collected labels: ===\n");
        for (int i = 0; i < label_count; i++)
            printf("[%02i]0x02%X: %s\n", i, labels[i]->address, labels[i]->id);
        printf("=========================\n");
    }

    /* Now go through file again, and write/validate instructions */
    create_asm(result, sasm, input_file, labels, label_count);
    return NULL;
}

sasm_label_t** parse_labels(sasm_parse_result_t* result, sasm_asm_t* sasm,
                            FILE* f, size_t* count)
{
    if (!result || !f || !count || !sasm)
        return NULL;

    rewind(f); /* Start at the beginning */
    *count = 0;
    sasm_label_t** labels = NULL;
    sasm_label_t* new_label = NULL;
    uint16_t addr = 0;
    char buf[LINE_LENGTH];

    while (fgets(buf, LINE_LENGTH, f) != NULL) {
        if (strlen(buf) < 1 || buf[0] == ';')
            continue;
        util_replace_char(buf, ';', '\0');
        util_trim_str(buf); /* Removes, comments, newline and leading spaces */

        /* Parse line to find out how much memory the instruction takes */
        sasm_mnemonic_type type = sasm_parse_type(sasm, buf);

        if (util_valid_mnemonic(type)) {
            switch (type) {
                case sasm_mnemonic_op:
                case sasm_mnemonic_fun: /* Fallthrough */
                    addr++;
                    break;
                case sasm_mnemonic_fun_int:
                case sasm_mnemonic_jump: /* Fallthrough */
                    addr += 2;
                    break;
                default:;
            }
        } else if (util_valid_label(buf)) {
            /* If it's not an instruction, but a label add it to the list */
            new_label = malloc(sizeof(sasm_label_t));
            new_label->address = addr;
            memcpy(new_label->id, buf, strlen(buf) + 1);

            labels = realloc(labels, (*count + 1) * sizeof(sasm_label_t*));
            labels[*count] = new_label;
            *count += 1;
        }
    }
    result->addr_space = addr;
    return labels;
}

void create_asm(sasm_parse_result_t* result, sasm_asm_t* sasm, FILE* f,
                sasm_label_t** labels, size_t label_count)
{
    if (!result || !f || !labels || !sasm)
        return;
    rewind(f); /* Start at the beginning */

    char buf[LINE_LENGTH];
    sasm_mnemonic_t* parsed_mnemonic = NULL;
    while (fgets(buf, LINE_LENGTH, f) != NULL) {
        if (strlen(buf) < 1 || buf[0] == ';')
            continue;
        util_replace_char(buf, ';', '\0');
        util_trim_str(buf); /* Removes, comments, newline and leading spaces */

        if (strlen(buf) < 1)
            continue;
        parsed_mnemonic = sasm_parse_line(sasm, buf);

        if (parsed_mnemonic) {

        } else {
            for (int i = 0; i < label_count; i++) {

            }
        }
    }
}
