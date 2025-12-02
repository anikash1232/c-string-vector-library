#include "gtest/gtest.h"

extern "C" {
#include "stdint.h"
#include "Vec.h"
}

/*
 * Unit tests established during the implementation of Vec.c
 * These tests are permitted to reach through the abstraction
 * and access members of a Vec struct directly.
 */

TEST(VecImpl, value) {
    Vec v = Vec_value(2, sizeof(int16_t));
    ASSERT_EQ(0, v.length);
    ASSERT_EQ(2, v.capacity);
    ASSERT_STREQ((char*)v.buffer, "");
    Vec_drop(&v);
}

TEST(VecImpl, drop) {
    Vec v = Vec_value(2, sizeof(int16_t));
    Vec_drop(&v);
    ASSERT_EQ(0, v.length);
    ASSERT_EQ(0, v.capacity);
    ASSERT_EQ(nullptr, v.buffer);
}

TEST(VecImpl, ref) {
    Vec v = Vec_value(2, sizeof(int16_t));
    int16_t *buffer = (int16_t*) v.buffer;
    buffer[0] = 1;
    buffer[1] = 2;
    v.length = 2;
    ASSERT_EQ(&buffer[0], Vec_ref(&v, 0));
    ASSERT_EQ(&buffer[1], Vec_ref(&v, 1));
    Vec_drop(&v);
}

TEST(VecImpl, ref_out_of_bounds) {
    Vec v = Vec_value(4, sizeof(int16_t));
    int16_t *buffer = (int16_t*) v.buffer;
    buffer[0] = 100;
    buffer[1] = 200;
    buffer[2] = 300;
    v.length = 3;
    ASSERT_DEATH({
       Vec_ref(&v, 3);
    }, ".* - Out of Bounds");
    Vec_drop(&v);
}

TEST(VecImpl, get_basic) {
    Vec v = Vec_value(2, sizeof(int));
    int *buffer = (int*)v.buffer;
    buffer[0] = 42;
    v.length = 1;

    int result = 0;
    Vec_get(&v, 0, &result);
    ASSERT_EQ(42, result);
    Vec_drop(&v);
}

TEST(VecImpl, set_replace_existing) {
    Vec v = Vec_value(2, sizeof(int));
    int *buffer = (int*)v.buffer;
    buffer[0] = 10;
    v.length = 1;

    int new_val = 99;
    Vec_set(&v, 0, &new_val);
    
    ASSERT_EQ(1, v.length);
    ASSERT_EQ(99, buffer[0]);
    Vec_drop(&v);
}

TEST(VecImpl, set_append) {
    Vec v = Vec_value(2, sizeof(int));
    int val = 5;
    
    Vec_set(&v, 0, &val);
    
    ASSERT_EQ(1, v.length);
    int *buffer = (int*)v.buffer;
    ASSERT_EQ(5, buffer[0]);
    Vec_drop(&v);
}


TEST(VecImpl, equals_true) {
    Vec v1 = Vec_value(5, sizeof(int));
    Vec v2 = Vec_value(5, sizeof(int));
    int val = 10;
    
    Vec_set(&v1, 0, &val);
    Vec_set(&v2, 0, &val);
    
    ASSERT_TRUE(Vec_equals(&v1, &v2));
    Vec_drop(&v1);
    Vec_drop(&v2);
}

TEST(VecImpl, equals_false_diff_length) {
    Vec v1 = Vec_value(5, sizeof(int));
    Vec v2 = Vec_value(5, sizeof(int));
    int val = 10;
    
    Vec_set(&v1, 0, &val);
    
    ASSERT_FALSE(Vec_equals(&v1, &v2));
    Vec_drop(&v1);
    Vec_drop(&v2);
}

TEST(VecImpl, equals_false_diff_content) {
    Vec v1 = Vec_value(5, sizeof(int));
    Vec v2 = Vec_value(5, sizeof(int));
    int val1 = 10;
    int val2 = 20;
    
    Vec_set(&v1, 0, &val1);
    Vec_set(&v2, 0, &val2);
    
    ASSERT_FALSE(Vec_equals(&v1, &v2));
    Vec_drop(&v1);
    Vec_drop(&v2);
}

TEST(VecImpl, splice_insert_empty) {
    Vec v = Vec_value(2, sizeof(int));
    int items[] = {1, 2};
    
    Vec_splice(&v, 0, 0, items, 2);
    
    ASSERT_EQ(2, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(1, buf[0]);
    ASSERT_EQ(2, buf[1]);
    Vec_drop(&v);
}

TEST(VecImpl, splice_insert_front) {
    Vec v = Vec_value(5, sizeof(int));
    int initial[] = {2, 3};
    Vec_splice(&v, 0, 0, initial, 2);
    
    int insert = 1;
    Vec_splice(&v, 0, 0, &insert, 1);
    
    ASSERT_EQ(3, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(1, buf[0]);
    ASSERT_EQ(2, buf[1]);
    ASSERT_EQ(3, buf[2]);
    Vec_drop(&v);
}


TEST(VecImpl, splice_insert_middle) {
    Vec v = Vec_value(5, sizeof(int));
    int initial[] = {1, 3};
    Vec_splice(&v, 0, 0, initial, 2);
    
    int insert = 2;
    Vec_splice(&v, 1, 0, &insert, 1);
    
    ASSERT_EQ(3, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(2, buf[1]);
    Vec_drop(&v);
}


TEST(VecImpl, splice_append) {
    Vec v = Vec_value(5, sizeof(int));
    int initial[] = {1, 2};
    Vec_splice(&v, 0, 0, initial, 2);
    
    int insert = 3;
    Vec_splice(&v, 2, 0, &insert, 1);
    
    ASSERT_EQ(3, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(3, buf[2]);
    Vec_drop(&v);
}


TEST(VecImpl, splice_delete_front) {
    Vec v = Vec_value(5, sizeof(int));
    int initial[] = {1, 2, 3};
    Vec_splice(&v, 0, 0, initial, 3);
    
    Vec_splice(&v, 0, 1, NULL, 0);
    
    ASSERT_EQ(2, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(2, buf[0]);
    ASSERT_EQ(3, buf[1]);
    Vec_drop(&v);
}

TEST(VecImpl, splice_delete_middle) {
    Vec v = Vec_value(5, sizeof(int));
    int initial[] = {1, 2, 3};
    Vec_splice(&v, 0, 0, initial, 3);
    
    Vec_splice(&v, 1, 1, NULL, 0);
    
    ASSERT_EQ(2, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(1, buf[0]);
    ASSERT_EQ(3, buf[1]);
    Vec_drop(&v);
}


TEST(VecImpl, splice_delete_end) {
    Vec v = Vec_value(5, sizeof(int));
    int initial[] = {1, 2, 3};
    Vec_splice(&v, 0, 0, initial, 3);
    
    Vec_splice(&v, 2, 1, NULL, 0);
    
    ASSERT_EQ(2, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(2, buf[1]);
    Vec_drop(&v);
}

TEST(VecImpl, splice_replace_overwrite) {
    Vec v = Vec_value(5, sizeof(int));
    int initial[] = {1, 99, 3};
    Vec_splice(&v, 0, 0, initial, 3);
    
    int replacement = 2;
    Vec_splice(&v, 1, 1, &replacement, 1);
    
    ASSERT_EQ(3, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(2, buf[1]);
    Vec_drop(&v);
}