#include "gtest/gtest.h"

extern "C" {
#include "Str.h"
#include "string.h"
}

TEST(StrImpl, value) {
    Str s = Str_value(2);
    ASSERT_EQ(1, s.length); // Vec's length includes null char
    ASSERT_EQ(3, s.capacity); // Requested space + 1 for null char
    char *buffer = (char*) s.buffer;
    ASSERT_EQ('\0', buffer[0]);
    Str_drop(&s);
}

TEST(StrImpl, drop) {
    Str s = Str_value(2);
    Str_drop(&s);
    ASSERT_EQ(0, s.capacity);
    ASSERT_EQ(0, s.length);
    ASSERT_EQ(nullptr, s.buffer);
}

/**
 * Helper fixture to setup a Str whose buffer
 * is prefilled with "abcd". 
 */
Str fixture_abcd()
{
    Str s = Str_value(4);
    char *buffer = (char*) s.buffer;
    buffer[0] = 'a';
    buffer[1] = 'b';
    buffer[2] = 'c';
    buffer[3] = 'd';
    buffer[4] = '\0';
    s.length = 5; // Includes null char
    return s;
}

TEST(StrImpl, length) {
    Str s = Str_value(2);
    ASSERT_EQ(0, Str_length(&s));
    Str_drop(&s);

    s = fixture_abcd();
    ASSERT_EQ(4, Str_length(&s));
    Str_drop(&s);
}

TEST(StrImpl, cstr) {
    Str s = fixture_abcd();
    ASSERT_STREQ("abcd", Str_cstr(&s));
    Str_drop(&s);
}

TEST(StrImpl, ref) {
    Str s = fixture_abcd();
    char *buffer = (char*) s.buffer;
    for (size_t i = 0; i < s.length; ++i) {
        ASSERT_EQ(&buffer[i], Str_ref(&s, i));
    }
    Str_drop(&s);
}

// TODO: Test remaining Str functions

TEST(StrImpl, from_basic) {
    Str s = Str_from("hello");

    ASSERT_EQ(6u, s.length); // hello + '\0'
    char* buffer = (char*) s.buffer;

    ASSERT_EQ('h', buffer[0]);
    ASSERT_EQ('e', buffer[1]);
    ASSERT_EQ('l', buffer[2]);
    ASSERT_EQ('l', buffer[3]);
    ASSERT_EQ('o', buffer[4]);
    ASSERT_EQ('\0', buffer[5]);

    Str_drop(&s);
}

TEST(StrImpl, get_basic) {
    Str s = Str_from("abc");

    ASSERT_EQ('a', Str_get(&s, 0));
    ASSERT_EQ('b', Str_get(&s, 1));
    ASSERT_EQ('c', Str_get(&s, 2));

    Str_drop(&s);
}

TEST(StrImpl, get_out_of_bounds) {
    Str s = Str_from("abc");

    ASSERT_DEATH({
        Str_get(&s, 3);
    }, ".* - Out of Bounds");

    Str_drop(&s);
}

TEST(StrImpl, set_overwrite_and_append) {
    Str s = Str_from("cat");

    Str_set(&s, 0, 'b');   // cat → bat
    Str_set(&s, 3, '!');   // bat → bat!

    char* buffer = (char*)s.buffer;

    ASSERT_EQ('b', buffer[0]);
    ASSERT_EQ('a', buffer[1]);
    ASSERT_EQ('t', buffer[2]);
    ASSERT_EQ('!', buffer[3]);
    ASSERT_EQ('\0', buffer[4]); // Null terminator always maintained

    ASSERT_EQ(5u, s.length);

    Str_drop(&s);
}

TEST(StrImpl, append_basic) {
    Str s = Str_from("car");

    Str_append(&s, "pet");  // car → carpet

    char* buffer = (char*)s.buffer;

    ASSERT_STREQ("carpet", buffer);
    ASSERT_EQ('\0', buffer[s.length - 1]);

    Str_drop(&s);
}

TEST(StrImpl, splice_insert_middle) {
    Str s = Str_from("hello");

    Str_splice(&s, 2, 0, "YY", 2); // heYYllo

    char* buffer = (char*)s.buffer;
    ASSERT_STREQ("heYYllo", buffer);
    ASSERT_EQ('\0', buffer[s.length - 1]);

    Str_drop(&s);
}

TEST(StrImpl, splice_delete_and_insert) {
    Str s = Str_from("abcdef");

    Str_splice(&s, 2, 3, "X", 1); 
    // delete c d e, insert X → abXf

    char* buffer = (char*)s.buffer;
    ASSERT_STREQ("abXf", buffer);
    ASSERT_EQ('\0', buffer[s.length - 1]);

    Str_drop(&s);
}

TEST(StrImpl, splice_oob_index) {
    Str s = Str_from("abc");

    ASSERT_DEATH({
        Str_splice(&s, 4, 0, "Z", 1);
    }, ".* - Out of Bounds");

    Str_drop(&s);
}

TEST(StrImpl, splice_oob_delete) {
    Str s = Str_from("abc");

    ASSERT_DEATH({
        Str_splice(&s, 1, 5, "Q", 1); 
    }, ".* - Out of Bounds");

    Str_drop(&s);
}
