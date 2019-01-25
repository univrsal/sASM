#include <string.h>
#include "asm.h"
#include "parser.h"

sasm_bool print_mnemonics = sasm_false;
sasm_bool debug_log = sasm_false;
char* mnemonics_path = NULL;
char* input_path = NULL;
char* output_path = NULL;

void print_help(void)
{
    printf("sASM - Simple Assembler\n");
    printf("Usage: -h print this help screen\n"
           "       -m [path to mnemonic definitions]\n"
           "       -i [path to assembler file]\n"
           "       -o [path to output binary file]\n"
           "       -p print out loaded mnemonics\n"
           "       -d enable debug log\n");
}

void parse_args(int argc, char** args)
{
    int help = 0;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(args[i], "-h")) {
            help = 1;
            break;
        } else if (!strcmp(args[i], "-p")) {
            print_mnemonics = 1;
        } else if (!strcmp(args[i], "-i")) {
            if (i + 1 < argc)
                input_path = args[++i];
        } else if (!strcmp(args[i], "-m")) {
            if (i + 1 < argc)
                mnemonics_path = args[++i];
        } else if (!strcmp(args[i], "-o")) {
            if (i + 1 < argc)
                output_path = args[++i];
        } else if (!strcmp(args[i], "-d")) {
            debug_log = sasm_true;
        }
    }

    if (help)
        print_help();
}

int main(int argc, char** args)
{
    if (argc > 1) {
        sasm_asm_t* asm_lang = NULL;
        parse_args(argc, args);

        if (mnemonics_path)
            asm_lang = sasm_asm_load(mnemonics_path);

        if (asm_lang && (print_mnemonics || debug_log))
            sasm_print_asm(asm_lang);

        if (asm_lang && input_path && output_path) {
            asm_lang->debug = debug_log;
            sasm_parse_result_t* result = sasm_build_asm(asm_lang,
                                                         input_path, output_path);
            if (result) {
                if (result->error_count > 0) {
                    printf("%i Error(s) occured:\n", result->error_count);
                    for (int i = 0; i < result->error_count; i++)
                    {
                        printf("[%03li] %s\n", result->errors[i]->line,
                               error_to_str(result->errors[i]->type));
                    }
                } else {
                    printf("Successfully parsed to %s\n", output_path);
                }
            }
            sasm_result_free(result);
        }
        else
        {
            if (!input_path)
                printf("!! Missing input file\n");

            if (!output_path)
                printf("!! Missing output file\n");
            print_help();
        }
        sasm_asm_free(asm_lang);
    } else {
        print_help();
    }
    return 0;
}