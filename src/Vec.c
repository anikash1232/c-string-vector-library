#include "Vec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Guards.h"


static void Vec_ensure_capacity(Vec *self, size_t min_capacity) {
    if (self->capacity >= min_capacity) {
        return;
    }

    size_t new_capacity = self->capacity == 0 ? 1 : self->capacity;
    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }

    void *new_buffer = calloc(new_capacity, self->item_size);
    OOM_GUARD(new_buffer, __FILE__, __LINE__);

    if (self->buffer != NULL && self->length > 0) {
        memcpy(new_buffer,
               self->buffer,
               self->length * self->item_size);
    }

    free(self->buffer);
    self->buffer = new_buffer;
    self->capacity = new_capacity;
}

/* Constructor / Destructor */

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

/* Accessors */

size_t Vec_length(const Vec* self) { return self->length; }

void* Vec_ref(const Vec* self, size_t index) {
    if (index < self->length) {
        return (char*)self->buffer + (index * self->item_size);
    } else {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}

/* Operations */

void Vec_get(const Vec* self, size_t index, void* out) {
    if (index >= self->length) {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    memcpy(out,
           (char*)self->buffer + index * self->item_size,
           self->item_size);
}

void Vec_set(Vec *self, size_t index, const void *value) {
    if (index > self->length) {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    size_t delete_count = (index < self->length) ? 1 : 0;
    size_t insert_count = 1;

    Vec_splice(self, index, delete_count, value, insert_count);
}


bool Vec_equals(const Vec* self, const Vec* other) {
    if (self->item_size != other->item_size) {
        return false;
    }
    if (self->length != other->length) {
        return false;
    }
    if (self->length == 0) {
        return true;
    }

    return memcmp(self->buffer,
                  other->buffer,
                  self->length * self->item_size) == 0;
}

void Vec_splice(Vec* self, size_t index, size_t delete_count, const void* items, size_t insert_count) {
    if (index > self->length || index + delete_count > self->length) {
        fprintf(stderr, "%s:%d - Out of Bounds", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    size_t old_length = self->length;
    size_t new_length = old_length - delete_count + insert_count;

    Vec_ensure_capacity(self, new_length);

    size_t elem_size = self->item_size;
    char* buffer = (char*)self->buffer;
    size_t tail_count = old_length - (index + delete_count);

    if (tail_count > 0 && (delete_count != insert_count)) {
        memmove(
            buffer + (index + insert_count) * elem_size,
            buffer + (index + delete_count) * elem_size,
            tail_count * elem_size
        );
    }

    if (insert_count > 0 && items != NULL) {
        memcpy(
            buffer + index * elem_size,
            items,
            insert_count * elem_size
        );
    }

    self->length = new_length;
}
