#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global Identifiers
char** commands = NULL;
typedef struct Variable {
    char* name;
    int value_i;
    char* value_s;
} variable;
variable** variables = NULL;

// Open File & Tokenizer
void free_memory() {
    if (commands) {
        for (int i = 0; i < 100; i++) {
            free(commands[i]);
        }
        free(commands);
    }

    if (variables) {
        for (int i = 0; i < 100; i++) {
            if (variables[i]) {
                free(variables[i]->name);
                free(variables[i]);
            }
        }
        free(variables);
    }
}

void OpenFile(const char* file_name) {
    if (commands) {
        for (int i = 0; i < 100; i++) {
            free(commands[i]);
        }
        free(commands);
    }

    if (!file_name || strlen(file_name) < 2) {
        fprintf(stderr, "error - please input a valid file name\n");
        return;
    }

    FILE* file = fopen(file_name, "r");
    if (!file) {
        fprintf(stderr, "error - invalid file (please check the file name)\n");
        return;
    }

    commands = calloc(100, sizeof(char*));
    if (!commands) {
        fprintf(stderr, "error - allocating commands array\n");
        fclose(file);
        return;
    }

    char command[257];
    int line_count = 0, index = 0;

    while (fgets(command, sizeof(command), file)) {
        line_count++;

        char* p = command, * q = command;
        while (*p) {
            if (*p != ' ' && *p != '\n' && *p != '\t') {
                *q++ = *p;
            }
            p++;
        }
        *q = '\0';

        if (command[0] == '\0' || command[0] == '/') {
            continue;
        }

        if (strchr(command, ';')) {
            if (index >= 99) {
                fprintf(stderr, "error - commands limit reached at line %d\n", line_count);
                break;
            }
            commands[index++] = _strdup(command);
        }
        else {
            fprintf(stderr, "error - missing semicolon at line %d\n", line_count);
        }
    }

    fclose(file);
}

void tokenizer() {
    if (!commands || !commands[0]) {
        fprintf(stderr, "error - tokenizing null commands array\n");
        return;
    }


    variables = calloc(100, sizeof(variable*));
    if (!variables) {
        fprintf(stderr, "error - failed to allocate variable memory array\n");
        return;
    }

    const char* keywords[10] = { "run", "print", "add" };
    char command[20];

    for (size_t i = 0; i < 100 && commands[i] != NULL; i++) {
        //add function

        if (sscanf(commands[i], "db %[^(]", command) == 1) {
            if (strcmp(command, "add") == 0) {
                int x, y;
                char var_name[50];

                if (sscanf(commands[i], "db add(%d, %d, %[^ )])", &x, &y, var_name) == 3) {
                    variable* var = malloc(sizeof(variable));
                    if (!var) {
                        fprintf(stderr, "error - failed to allocate variable memory\n");
                        continue;
                    }

                    var->value_i = x + y;
                    var->name = _strdup(var_name);

                    if (!var->name) {
                        free(var);
                        continue;
                    }

                    for (size_t j = 0; j < 100; j++) {
                        if (!variables[j]) {
                            variables[j] = var;
                            break;
                        }
                    }
                }
                else {
                    fprintf(stderr, "error - syntax error in add command\n");
                    break;
                }
            }

            //print function

            else if (strcmp(command, "print") == 0) {
                char buffer[50];
                int found = 0;

                if (sscanf(commands[i], "db print(%[^ )]", buffer) == 1) {

                        for (size_t j = 0; j < 100 && variables[j] != NULL; j++) {

                            if (strcmp(buffer, variables[j]->name) == 0) {

                                found = 1;

                                if (variables[j]->value_i != NULL) {
                                    printf("%d\n",variables[j]->value_i);
                                    break;
                                }

                                else if (variables[j]->value_s != NULL) {
                                    printf("%s\n",variables[j]->value_s);
                                    break;
                                }

                                else {
                                    fprintf(stderr, "error - uninitialized variable %s\n", buffer);
                                    break;
                                }
                            }
                        }
                        if (found != 1) {
                            fprintf(stderr, "error - undefined variable %s\n", buffer);
                            break;
                        }

                } else {
                        fprintf(stderr, "error - syntax error in print command\n");
                        break;
                    }
                }
            }
        }
    }


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "error - please provide a file: %s file.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    OpenFile(argv[1]);
    tokenizer();
    free_memory();

    return EXIT_SUCCESS;
}
