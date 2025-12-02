#include "Str.h"

#include <string.h>

#include "Vec.h"

static char NULL_CHAR = '\0';

Str Str_value(size_t capacity) {
    Str s = Vec_value(capacity + 1, sizeof(char));
    // TODO: Replace the below lines with a call below to Vec_set
    // once you have Vec_set correctly implemented
    s.length = 1;
    char* buffer = (char*)s.buffer;
    buffer[0] = NULL_CHAR;
    // Vec_set(&s, 0, &NULL_CHAR);
    return s;
}

void Str_drop(Str* self) { Vec_drop(self); }

size_t Str_length(const Str* self) { return Vec_length(self) - 1; }

const char* Str_cstr(const Str* self) { return (char*)Vec_ref(self, 0); }

char* Str_ref(const Str* self, const size_t index) {
    return (char*)Vec_ref(self, index);
}

Str Str_from(const char* cstr) {
    (void)cstr;
    Str s = {0};
    return s;
}

void Str_splice(Str* self, size_t index, size_t delete_count, const char* cstr,
                size_t insert_count) {
    (void)self;
    (void)index;
    (void)delete_count;
    (void)cstr;
    (void)insert_count;
}

void Str_append(Str* self, const char* cstr) {
    (void)self;
    (void)cstr;
}

char Str_get(const Str* self, size_t index) {
    (void)self;
    (void)index;
    return NULL_CHAR;
}

void Str_set(Str* self, size_t index, const char value) {
    (void)self;
    (void)index;
    (void)value;
}