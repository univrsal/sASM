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
#include <stdlib.h>
#include "util.h"
#include "parser.h"
#include "asm.h"

/* Frees struct as well as included labels and errors */
void sasm_result_free(sasm_parse_result_t* r)
{
    if (!r)
        return;

    int i;
    if (r->errors) {
        for (i = 0; i < r->error_count; i++)
            free(r->errors[i]);
        free(r->errors);
    }

    if (r->labels) {
        for (i = 0; i < r->label_count; i++)
            free(r->labels[i]);
        free(r->labels);
    }

    free(r);
}

/* Creates a rom file from an assembly file and an assembler language */
sasm_parse_result_t* sasm_build_asm(sasm_asm_t* sasm, const char* input, const char* output)
{
    /* Input sanitizing and preparing */
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
    FILE* output_file = fopen(output, "w");

    if (!input_file || !output_file) {
        printf("Error opening file: In: %s, Out: %s\n", input_file ?
                                                        "OK" : "Error", output_file ? "OK" : "Error");
        return NULL;
    }

    sasm_parse_result_t* result = malloc(sizeof(sasm_parse_result_t));
    result->error_count = 0;
    result->addr_space = 0;
    result->label_count = 0;
    result->errors = NULL;
    result->labels = NULL;

    /* Collect all jump labels in input file */
    parse_labels(result, sasm, input_file);

    /* Log all collected labels */
    if (sasm->debug) {
        printf("+-- Collected labels\n| Target address | Name\n| Last address: 0x%03llX\n",
               result->addr_space);
        for (int i = 0; i < result->label_count; i++)
            printf("| 0x%02X: %s\n", result->labels[i]->address, result->labels[i]->id);
    }

    /* Go through file again and write each instruction to rom file  */
    create_asm(result, sasm, input_file, output_file);

    fclose(input_file);
    fclose(output_file);
    return result;
}

/* Find and collect all labels in input file */
void parse_labels(sasm_parse_result_t* result, sasm_asm_t* sasm, FILE* f)
{
    if (!result || !f || !sasm)
        return;

    rewind(f); /* Start at the beginning */
    sasm_label_t* new_label = NULL;
    uint16_t addr = 0;
    size_t line = 0;
    char buf[LINE_LENGTH];

    if (sasm->debug)
        printf("+--- Parsing -------\n| Address | Mnemonic\n");

    while (fgets(buf, LINE_LENGTH, f) != NULL) {
        line++;
        util_trim_str(buf); /* Removes, comments, newline and leading spaces */

        /* Ignore emtpy lines (Pure comment lines were cut by util_trim_str */
        if (strlen(buf) < 1)
            continue;

        /* Parse instruction to find out how much memory the it takes */
        sasm_mnemonic_type type = sasm_parse_type(sasm, buf);

        if (util_valid_mnemonic(type)) {
            if (sasm->debug)
                printf("| [0x%02X] %s\n", addr, buf);
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
            buf[strlen(buf) - 1] = '\0'; /* Cut off ':' */

            /* First check if the label already exists */
            if (is_label_duplicate(result, buf)) {
                add_error(result, sasm_duplicate_label, line);
            } else if (check_label_conflict(sasm, buf)) {
                add_error(result, sasm_label_conflict, line);
            } else {
                /* Add the new label */
                new_label = malloc(sizeof(sasm_label_t));
                new_label->address = addr;
                memcpy(new_label->id, buf, strlen(buf) + 1);
                result->labels = realloc(result->labels,
                                         (result->label_count + 1) * sizeof(sasm_label_t*));
                result->labels[result->label_count] = new_label;
                result->label_count++;
            }
        }
    }

    result->addr_space = addr;
}

/* Writes asm file into rom file */
void create_asm(sasm_parse_result_t *result, sasm_asm_t *sasm, FILE *ifp, FILE *ofp)
{
    if (!result || !ifp || !sasm)
        return;

    rewind(ifp); /* Start at the beginning */
    size_t line = 0;
    /* Counts amount of bytes written, since rom file only contains 8 bytes per line */
    int byte_count = 0;
    char buf[LINE_LENGTH];
    char** splits = NULL; /* Contains current line divided by spaces */
    sasm_mnemonic_t* parsed_mnemonic = NULL;

    fprintf(ofp, "v2.0 raw\n"); /* Write header */

    if (sasm->debug)
        printf("+-- File writing\n");

    while (fgets(buf, LINE_LENGTH, ifp) != NULL) {
        line++;
        util_trim_str(buf); /* Removes comments, newline and leading spaces */

        if (strlen(buf) < 1)
            continue;

        parsed_mnemonic = sasm_parse_line(sasm, buf, &splits);

        if (parsed_mnemonic) {
            if (sasm->debug)
                printf("| [%02lli] %-6s %i: 0x%02X", line, splits[0],
                       parsed_mnemonic->type, parsed_mnemonic->op_code);

            uint16_t jmp_target = 0; /* Contains address of label */

            /* parse_line verified all types except jumps -> Verify jumps */
            if (parsed_mnemonic->type == sasm_mnemonic_jump) {
                sasm_bool valid_jump = sasm_false;
                if (util_valid_hex(splits[1])) {
                    /* This jump instruction jumps directly to an address */
                    if (strtol(buf, NULL, 16) <= result->addr_space)
                        valid_jump = sasm_true;
                    else
                        add_error(result, sasm_out_of_range_jump, line);
                } else {
                    /* Jump argument is not an address, so assume it's a label
                     * and try to find a label with a matching name
                     */
                    for (int i = 0; i < result->label_count; i++) {
                        if (splits && splits[1] &&
                            !strcmp(result->labels[i]->id, splits[1])) {
                            jmp_target = result->labels[i]->address;
                            valid_jump = sasm_true;
                            if (sasm->debug)
                                printf(", jumps to %-8s(0x%02X)",
                                       result->labels[i]->id, result->labels[i]->address);

                            break;
                        }
                    }
                }

                if (!valid_jump) {
                    add_error(result, sasm_invalid_jump, line);
                    util_free_strings(splits);

                    if (sasm->debug)
                        printf("\n");
                    continue;
                }
            }

            if (sasm->debug)
                printf("\n");

            /* The mnemonic could be parsed -> Write it to file */
            fprintf(ofp, "%x", parsed_mnemonic->op_code);
            add_op_count(ofp, &byte_count);
            uint8_t parsed_int = 0;

            switch (parsed_mnemonic->type) {
                case sasm_mnemonic_jump:
                    fprintf(ofp, "%x", jmp_target);
                    add_op_count(ofp, &byte_count);
                    break;
                case sasm_mnemonic_fun_int:
                    if (util_parse_int(splits[1], &parsed_int)) {
                        fprintf(ofp, "%x", parsed_int);
                        add_op_count(ofp, &byte_count);
                    } else {
                        add_error(result, sasm_int_parsing, line);
                    }
                    break;
                default: ;
            }
        } else { /* It's not a mnemonic -> assume it's a label */
            sasm_bool valid_label = sasm_false;

            if (util_valid_label(buf)) {
                buf[strlen(buf) - 1] = '\0'; /* remove ':' */
                for (int i = 0; i < result->label_count; i++) {
                    if (!strcmp(buf, result->labels[i]->id)) {
                        valid_label = sasm_true;
                        break;
                    }
                }
            }

            if (!valid_label)
                add_error(result, sasm_unkown_op, line);
        }

        util_free_strings(splits);
    }

    if (sasm->debug)
        printf("+-------------------/\n");
    fprintf(ofp, "\n");
}

void add_error(sasm_parse_result_t* result, sasm_error_code err, size_t line)
{
    result->errors = realloc(result->errors, (result->error_count + 1) * sizeof(sasm_parse_error_t*));
    result->errors[result->error_count] = malloc(sizeof(sasm_parse_error_t));
    result->errors[result->error_count]->line = line; /* starts counting at 0 */
    result->errors[result->error_count]->type = err;
    result->error_count++;
}

const char* error_to_str(sasm_error_code err)
{
    switch (err) {
        default:
        case sasm_general_failure:
            return "Unknown error";
        case sasm_unkown_op:
            return "Unknown operation";
        case sasm_out_of_range_jump:
            return "Address points out of range";
        case sasm_duplicate_label:
            return "Duplicate label name";
        case sasm_label_conflict:
            return "Label name conflicts with mnemonic name";
        case sasm_invalid_jump:
            return "Invalid jump";
    }
}

void add_op_count(FILE* of, int* count)
{
    *count += 1;
    if (*count >= 8) {
        *count = 0;
        fprintf(of, "\n");
    } else {
        fprintf(of, " ");
    }
}

sasm_bool check_label_conflict(sasm_asm_t* sasm, const char* label)
{
    int i;
    for (i = 0; i < sasm->mnemonic_count; i++) {
        if (!strcmp(sasm->mnemonics[i]->id, label)
            || !strcmp(sasm->mnemonics[i]->arg, label)) {
            return sasm_true;
        }
    }
    return sasm_false;
}

sasm_bool is_label_duplicate(sasm_parse_result_t* result, const char* label)
{
    int i;
    for (i = 0; i < result->label_count; i++)
        if (!strcmp(label, result->labels[i]->id))
            return sasm_true;
    return sasm_false;
}
