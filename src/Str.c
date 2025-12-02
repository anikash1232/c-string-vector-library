#include "Str.h"

#include <string.h>
#include <stdio.h>

static const char NULL_CHAR = '\0';

Str Str_value(size_t capacity) {
    Str s = Vec_value(capacity + 1, sizeof(char));
    Vec_set(&s, 0, &NULL_CHAR);
    return s;
}

void Str_drop(Str* self) {
    Vec_drop(self);
}

size_t Str_length(const Str* self) {
    return Vec_length(self) - 1;
}

char* Str_ref(const Str* self, size_t index) {
    return (char*)Vec_ref(self, index);
}

const char* Str_cstr(const Str* self) {
    return (const char*)Vec_ref(self, 0);
}

Str Str_from(const char* cstr) {
    size_t n = strlen(cstr);
    Str s = Str_value(n);

    for (size_t i = 0; i < n; i++) {
        Vec_set(&s, i, &cstr[i]);
    }

    Vec_set(&s, n, &NULL_CHAR);
    return s;
}

void Str_splice(Str* self, size_t index, size_t delete_count,
                const char* cstr, size_t insert_count) {

    size_t len = Str_length(self);

    if (index > len || index + delete_count > len) {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    Vec_splice(self, index, delete_count, cstr, insert_count);

    size_t new_len = len - delete_count + insert_count;
    Vec_set(self, new_len, &NULL_CHAR);
}

void Str_append(Str* self, const char* cstr) {
    size_t n = strlen(cstr);
    size_t start = Str_length(self);
    Str_splice(self, start, 0, cstr, n);
}

char Str_get(const Str* self, size_t index) {
    if (index >= Str_length(self)) {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    char out;
    Vec_get(self, index, &out);
    return out;
}

void Str_set(Str* self, size_t index, const char value) {
    size_t len = Str_length(self);

    if (index > len) {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    if (index == len) {
        Str_splice(self, index, 0, &value, 1);
    } else {
        Str_splice(self, index, 1, &value, 1);
    }
}