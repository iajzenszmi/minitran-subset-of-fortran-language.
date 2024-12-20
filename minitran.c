#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define the maximum number of lines and line length
#define MAX_LINES 100
#define MAX_LINE_LENGTH 256

// Structure to represent a program
typedef struct {
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_count;
} Program;

// Function to parse a Minitran program from a file
Program *parse_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    Program *program = (Program *)malloc(sizeof(Program));
    program->line_count = 0;

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Ignore comments (lines starting with 'C') and blank lines
        if (line[0] == 'C' || line[0] == '\n') continue;
        strncpy(program->lines[program->line_count++], line, MAX_LINE_LENGTH);
    }

    fclose(file);
    return program;
}

// Function to free memory used by the program structure
void free_program(Program *program) {
    if (program) {
        free(program);
    }
}

// Function to evaluate an expression (supports addition)
int evaluate_expression(char *expr, int vars[26]) {
    int result = 0;
    char *token = strtok(expr, "+");
    while (token != NULL) {
        // Trim leading and trailing whitespace
        while (isspace(*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace(*end)) {
            *end = '\0';
            end--;
        }
        // Check if token is a variable
        if (isalpha(*token)) {
            char var = toupper(*token);
            result += vars[var - 'A'];
        } else {
            // Assume it's a constant integer
            result += atoi(token);
        }
        token = strtok(NULL, "+");
    }
    return result;
}

// Function to execute the parsed Minitran program
void execute_program(Program *program) {
    int vars[26] = {0}; // Variable storage for A-Z

    for (int i = 0; i < program->line_count; i++) {
        char *line = program->lines[i];

        // Handle WRITE statement
        if (strncmp(line, "WRITE", 5) == 0) {
            char var = toupper(line[6]); // Assume format: WRITE A
            printf("%d\n", vars[var - 'A']);
        }
        // Handle variable assignment (e.g., A = 5 or B = A + 10)
        else if (strchr(line, '=')) {
            char var = toupper(line[0]); // Assume variable name is a single character
            char expr[MAX_LINE_LENGTH];
            strcpy(expr, line + 4); // Copy expression after '='

            // Remove newline character if present
            char *newline = strchr(expr, '\n');
            if (newline) *newline = '\0';

            int value = evaluate_expression(expr, vars);
            vars[var - 'A'] = value;
        }
        // Handle END statement
        else if (strncmp(line, "END", 3) == 0) {
            break;
        }
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <program.mt>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    Program *program = parse_file(filename);

    if (!program) {
        fprintf(stderr, "Error: Failed to parse file.\n");
        return 1;
    }

    // printf("Running program...\n");
    execute_program(program);
    free_program(program);

    return 0;
}
