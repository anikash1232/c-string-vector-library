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
    Vec v = Vec_value(4, sizeof(int));
    int *buf = (int*)v.buffer;

    buf[0] = 10;
    buf[1] = 20;
    buf[2] = 30;
    v.length = 3;

    int out = 0;
    Vec_get(&v, 1, &out);
    ASSERT_EQ(20, out);

    Vec_drop(&v);
}

TEST(VecImpl, get_out_of_bounds) {
    Vec v = Vec_value(2, sizeof(int));
    int *buf = (int*)v.buffer;
    buf[0] = 1;
    v.length = 1;

    int out = 0;
    ASSERT_DEATH({
        Vec_get(&v, 1, &out);
    }, ".* - Out of Bounds");

    Vec_drop(&v);
}

TEST(VecImpl, set_overwrite_and_append) {
    Vec v = Vec_value(2, sizeof(int));
    int x = 5;
    int y = 10;
    int z = 20;

    Vec_set(&v, 0, &x);
    Vec_set(&v, 1, &y);

    ASSERT_EQ(2u, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(5, buf[0]);
    ASSERT_EQ(10, buf[1]);

    Vec_set(&v, 0, &z);
    ASSERT_EQ(2u, v.length);
    ASSERT_EQ(20, buf[0]);
    ASSERT_EQ(10, buf[1]);

    Vec_drop(&v);
}

TEST(VecImpl, set_append_grows_capacity) {
    Vec v = Vec_value(1, sizeof(int));
    int values[3] = {1, 2, 3};

    Vec_set(&v, 0, &values[0]);
    Vec_set(&v, 1, &values[1]);
    Vec_set(&v, 2, &values[2]);

    ASSERT_GE(v.capacity, 3u);
    ASSERT_EQ(3u, v.length);
    int *buf = (int*)v.buffer;
    ASSERT_EQ(1, buf[0]);
    ASSERT_EQ(2, buf[1]);
    ASSERT_EQ(3, buf[2]);

    Vec_drop(&v);
}

TEST(VecImpl, set_out_of_bounds) {
    Vec v = Vec_value(2, sizeof(int));
    int x = 42;

    ASSERT_DEATH({
        Vec_set(&v, 1, &x);
    }, ".* - Out of Bounds");

    Vec_drop(&v);
}

TEST(VecImpl, equals_basic_true_and_false) {
    Vec a = Vec_value(4, sizeof(int));
    Vec b = Vec_value(4, sizeof(int));

    int *abuf = (int*)a.buffer;
    int *bbuf = (int*)b.buffer;

    abuf[0] = 1; abuf[1] = 2; abuf[2] = 3;
    bbuf[0] = 1; bbuf[1] = 2; bbuf[2] = 3;
    a.length = b.length = 3;

    ASSERT_TRUE(Vec_equals(&a, &b));

    bbuf[1] = 99;
    ASSERT_FALSE(Vec_equals(&a, &b));

    b.length = 2;
    ASSERT_FALSE(Vec_equals(&a, &b));

    Vec_drop(&a);
    Vec_drop(&b);
}

TEST(VecImpl, splice_insert_only_middle) {
    Vec v = Vec_value(4, sizeof(int));
    int *buf = (int*)v.buffer;
    buf[0] = 100;
    buf[1] = 200;
    buf[2] = 300;
    buf[3] = 400;
    v.length = 4;

    int a[2] = {800, 900};

    Vec_splice(&v, 2, 0, a, 2);
    ASSERT_EQ(6u, v.length);
    buf = (int*)v.buffer;
    int expected1[6] = {100, 200, 800, 900, 300, 400};
    for (size_t i = 0; i < 6; i++) {
        ASSERT_EQ(expected1[i], buf[i]);
    }

    Vec_drop(&v);
}

TEST(VecImpl, splice_replace_one_element) {
    Vec v = Vec_value(4, sizeof(int));
    int *buf = (int*)v.buffer;
    buf[0] = 100;
    buf[1] = 200;
    buf[2] = 300;
    buf[3] = 400;
    v.length = 4;

    int a[2] = {800, 900};

    Vec_splice(&v, 2, 1, a, 2);
    ASSERT_EQ(5u, v.length);
    buf = (int*)v.buffer;
    int expected2[5] = {100, 200, 800, 900, 400};
    for (size_t i = 0; i < 5; i++) {
        ASSERT_EQ(expected2[i], buf[i]);
    }

    Vec_drop(&v);
}

TEST(VecImpl, splice_delete_prefix_and_insert_one) {
    Vec v = Vec_value(4, sizeof(int));
    int *buf = (int*)v.buffer;
    buf[0] = 100;
    buf[1] = 200;
    buf[2] = 300;
    buf[3] = 400;
    v.length = 4;

    int a[1] = {800};

    Vec_splice(&v, 0, 3, a, 1);
    ASSERT_EQ(2u, v.length);
    buf = (int*)v.buffer;
    int expected3[2] = {800, 400};
    for (size_t i = 0; i < 2; i++) {
        ASSERT_EQ(expected3[i], buf[i]);
    }

    Vec_drop(&v);
}

TEST(VecImpl, splice_out_of_bounds_index) {
    Vec v = Vec_value(2, sizeof(int));
    int *buf = (int*)v.buffer;
    buf[0] = 1;
    buf[1] = 2;
    v.length = 2;

    int x[1] = {10};

    ASSERT_DEATH({
        Vec_splice(&v, 3, 0, x, 1);
    }, ".* - Out of Bounds");

    Vec_drop(&v);
}

TEST(VecImpl, splice_out_of_bounds_delete) {
    Vec v = Vec_value(3, sizeof(int));
    int *buf = (int*)v.buffer;
    buf[0] = 1;
    buf[1] = 2;
    buf[2] = 3;
    v.length = 3;

    int x[1] = {10};

    ASSERT_DEATH({
        Vec_splice(&v, 1, 3, x, 1);
    }, ".* - Out of Bounds");

    Vec_drop(&v);
}
