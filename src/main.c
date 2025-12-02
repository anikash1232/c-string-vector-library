#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Vec.h"
#include "Str.h"


#define CHUNK_SIZE 10

int main() {
    char buffer[CHUNK_SIZE];
    
    while (1) {
        Str line = Str_value(CHUNK_SIZE);
        bool eof = false;
        bool newline_found = false;

        while (fgets(buffer, CHUNK_SIZE, stdin) != NULL) {
            Str_append(&line, buffer);
            
            const char* current = Str_cstr(&line);
            size_t len = Str_length(&line);
            
            if (len > 0 && current[len - 1] == '\n') {
                newline_found = true;
                break;
            }
        }
        
        if (!newline_found && feof(stdin)) {
            if (Str_length(&line) > 0) {
                 printf("%s", Str_cstr(&line));
            }
            Str_drop(&line);
            break;
        }

        printf("%s", Str_cstr(&line));
        
        Str_drop(&line);
    }

    return EXIT_SUCCESS;
}