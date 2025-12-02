#include <stdio.h>
#include <stdlib.h>
#include "Str.h"

int main(void) {
    char chunk[128];
    Str line = Str_value(16);

    while (fgets(chunk, sizeof(chunk), stdin) != NULL) {
        Str_append(&line, chunk);
        if (chunk[strlen(chunk) - 1] == '\n' || feof(stdin)) {
            printf("%s", Str_cstr(&line));
            Str_drop(&line);
            line = Str_value(16);
        }
    }

    Str_drop(&line);

    return EXIT_SUCCESS;
}
