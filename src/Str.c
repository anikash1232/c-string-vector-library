#include "Str.h"
#include <string.h>
#include "Vec.h"

static char NULL_CHAR = '\0';

Str Str_value(size_t capacity) {
    Str s = Vec_value(capacity + 1, sizeof(char));
    Vec_set(&s, 0, &NULL_CHAR);
    return s;
}

void Str_drop(Str* self) { Vec_drop(self); }

size_t Str_length(const Str* self) { 
    return Vec_length(self) - 1; 
}

const char* Str_cstr(const Str* self) { 
    return (char*)Vec_ref(self, 0); 
}

char* Str_ref(const Str* self, const size_t index) {
    return (char*)Vec_ref(self, index);
}

Str Str_from(const char* cstr) {
    size_t len = strlen(cstr);
    Str s = Str_value(len);
    Vec_splice(&s, 0, 0, cstr, len);
    return s;
}

void Str_splice(Str* self, size_t index, size_t delete_count, const char* cstr,
                size_t insert_count) {
    Vec_splice(self, index, delete_count, cstr, insert_count);
}

void Str_append(Str* self, const char* cstr) {
    size_t len = strlen(cstr);
    Str_splice(self, Str_length(self), 0, cstr, len);
}

char Str_get(const Str* self, size_t index) {
    char c;
    Vec_get(self, index, &c);
    return c;
}

void Str_set(Str* self, size_t index, const char value) {
    if (index == Str_length(self)) {
        Str_splice(self, index, 0, &value, 1);
    } else {
        Vec_set(self, index, &value);
    }
}