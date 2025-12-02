#include "Vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Guards.h"


Vec Vec_value(size_t capacity, size_t item_size) {
    Vec vec = {item_size, 0, capacity, calloc(capacity, item_size)};
    OOM_GUARD(vec.buffer, __FILE__, __LINE__);
    return vec;
}

void Vec_drop(Vec* self) {
    free(self->buffer);
    self->buffer = NULL;
    self->capacity = 0;
    self->length = 0;
}


size_t Vec_length(const Vec* self) { return self->length; }

void* Vec_ref(const Vec* self, size_t index) {
    if (index < self->length) {
        return (char*)self->buffer + (index * self->item_size);
    } else {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}


void Vec_get(const Vec* self, size_t index, void* out) {
    void* ptr = Vec_ref(self, index);
    memcpy(out, ptr, self->item_size);
}

void Vec_splice(Vec* self, size_t index, size_t delete_count, const void* items,
                size_t insert_count) {
    if (index + delete_count > self->length) {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    size_t new_length = self->length - delete_count + insert_count;

    if (new_length > self->capacity) {
        size_t new_capacity = self->capacity == 0 ? 1 : self->capacity * 2;
        while (new_capacity < new_length) {
            new_capacity *= 2;
        }
        self->buffer = realloc(self->buffer, new_capacity * self->item_size);
        OOM_GUARD(self->buffer, __FILE__, __LINE__);
        self->capacity = new_capacity;
    }

    char* dest = (char*)self->buffer + (index + insert_count) * self->item_size;

    char* src = (char*)self->buffer + (index + delete_count) * self->item_size;

    size_t bytes_to_move = (self->length - (index + delete_count)) * self->item_size;

    if (bytes_to_move > 0) {
        memmove(dest, src, bytes_to_move);
    }


    if (insert_count > 0 && items != NULL) {
        char* insert_pos = (char*)self->buffer + (index * self->item_size);
        memcpy(insert_pos, items, insert_count * self->item_size);
    }

    self->length = new_length;
}

void Vec_set(Vec* self, size_t index, const void* value) {    
    if (index == self->length) {
        Vec_splice(self, index, 0, value, 1);
    } else if (index < self->length) {
        Vec_splice(self, index, 1, value, 1);
    } else {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}

bool Vec_equals(const Vec* self, const Vec* other) {
    if (self->length != other->length) {
        return false;
    }
    if (self->item_size != other->item_size) {
        return false;
    }
    return memcmp(self->buffer, other->buffer, self->length * self->item_size) == 0;
}