# String and Vector Library

Generic dynamic array and string types in C, with an allocation-guard layer — the data
structures a higher-level language gives you, implemented from scratch over `malloc`.

## What it does

`Vec` is a growable array that stores elements of *any* type. Since C has no generics, it
works in terms of `void *` and a per-instance `item_size` fixed at construction, copying
bytes in and out rather than storing typed values. `Str` builds a dynamic string on top of
the same idea.

```c
Vec v = Vec_value(16, sizeof(int));   // capacity 16, elements of int size
int x = 42;
Vec_set(&v, 0, &x);
Vec_get(&v, 0, &out);
Vec_drop(&v);                          // caller owns the lifetime
```

## API

```c
Vec    Vec_value(size_t capacity, size_t item_size);
void   Vec_drop(Vec *self);
size_t Vec_length(const Vec *self);
void*  Vec_ref(const Vec *self, size_t index);      // borrow, no copy
void   Vec_get(const Vec *self, size_t index, void *out);   // copy out
void   Vec_set(Vec *self, size_t index, const void *value); // copy in
bool   Vec_equals(const Vec *self, const Vec *other);
void   Vec_splice(...);                              // insert/remove ranges
```

`Vec_ref` hands back a pointer into the vector's own storage — fast, but invalidated by any
growth. `Vec_get` copies into caller-owned memory and stays valid. Having both lets a caller
choose between speed and safety explicitly rather than guessing which one the library gives.

## Structure

```
include/Vec.h      generic dynamic array
include/Str.h      dynamic string
include/Guards.h   OOM_GUARD - allocation failure check with file and line
src/Vec.c, Str.c, Guards.c
src/main.c         driver
```

Every allocation routes through `OOM_GUARD`, which takes `__FILE__` and `__LINE__` so a
failure reports where it happened instead of dying anonymously.

## Building

```bash
gcc -Wall -Wextra -Iinclude src/*.c -o main
```
