#include "gtest/gtest.h"

extern "C" {
#include "stdint.h"
#include "Str.h"
}

/**
 * The purpose of these tests is to prove correctness of the Str
 * abstraction barrier from the user's point-of-view.
 */

TEST(StrSpec, values_init_empty) {
    Str s = Str_value(10);
    ASSERT_EQ(0, Str_length(&s));
    Str_drop(&s);
}

TEST(StrSpec, values_init_cstr) {
    Str s = Str_value(10);
    ASSERT_STREQ("", Str_cstr(&s));
    Str_drop(&s);
}

TEST(StrSpec, from_basic) {
    Str s = Str_from("hello");
    ASSERT_EQ(5u, Str_length(&s));
    ASSERT_STREQ("hello", Str_cstr(&s));
    Str_drop(&s);
}

TEST(StrSpec, get_characters) {
    Str s = Str_from("abc");
    ASSERT_EQ('a', Str_get(&s, 0));
    ASSERT_EQ('b', Str_get(&s, 1));
    ASSERT_EQ('c', Str_get(&s, 2));
    Str_drop(&s);
}

TEST(StrSpec, set_overwrite_and_append) {
    Str s = Str_from("cat");

    Str_set(&s, 0, 'b');   // overwrite 'c' → 'b'
    ASSERT_STREQ("bat", Str_cstr(&s));

    Str_set(&s, 3, '!');   // append at end
    ASSERT_STREQ("bat!", Str_cstr(&s));

    Str_drop(&s);
}

TEST(StrSpec, append_basic) {
    Str s = Str_from("car");
    Str_append(&s, "pet");
    ASSERT_STREQ("carpet", Str_cstr(&s));
    ASSERT_EQ(6u, Str_length(&s));
    Str_drop(&s);
}

TEST(StrSpec, splice_insert_middle) {
    Str s = Str_from("hello");
    Str_splice(&s, 2, 0, "YY", 2);
    ASSERT_STREQ("heYYllo", Str_cstr(&s));
    Str_drop(&s);
}

TEST(StrSpec, splice_delete_and_insert) {
    Str s = Str_from("abcdef");
    Str_splice(&s, 2, 3, "X", 1);
    ASSERT_STREQ("abXf", Str_cstr(&s));
    ASSERT_EQ(4u, Str_length(&s));
    Str_drop(&s);
}

TEST(StrSpec, append_empty_string) {
    Str s = Str_from("test");
    Str_append(&s, "");
    ASSERT_STREQ("test", Str_cstr(&s));
    ASSERT_EQ(4u, Str_length(&s));
    Str_drop(&s);
}

TEST(StrSpec, splice_insert_zero) {
    Str s = Str_from("data");
    Str_splice(&s, 2, 0, "", 0);
    ASSERT_STREQ("data", Str_cstr(&s));
    Str_drop(&s);
}

