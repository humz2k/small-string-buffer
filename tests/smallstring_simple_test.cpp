#include <gtest/gtest.h>
#include <smallstring/smallstring.hpp>

struct smallstring_simple_test_fixture : public ::testing::Test {
    smallstring::Buffer<> buffer;
};

TEST_F(smallstring_simple_test_fixture, empty_buffer_is_empty) {
    EXPECT_EQ(buffer.view(), "");
    EXPECT_EQ(buffer.length(), 0UL);
}

TEST_F(smallstring_simple_test_fixture, append_string_literal) {
    buffer.push("test");
    EXPECT_EQ(buffer.view(), "test");
    EXPECT_EQ(buffer.length(), 4UL);
}

TEST_F(smallstring_simple_test_fixture, append_char_ptr) {
    const char* to_add = "test1234";
    buffer.push(to_add, 8);
    EXPECT_EQ(buffer.view(), "test1234");
    EXPECT_EQ(buffer.length(), 8UL);
}

TEST_F(smallstring_simple_test_fixture, append_string_view) {
    std::string_view to_add = "yeettest1234";
    buffer.push(to_add);
    EXPECT_EQ(buffer.view(), "yeettest1234");
    EXPECT_EQ(buffer.length(), 12UL);
}

TEST_F(smallstring_simple_test_fixture, append_string) {
    std::string to_add = "thisisastring";
    buffer.push(to_add);
    EXPECT_EQ(buffer.view(), "thisisastring");
    EXPECT_EQ(buffer.length(), 13UL);
}

TEST_F(smallstring_simple_test_fixture, append_int) {
    int to_add = 5;
    buffer.push(to_add);
    EXPECT_EQ(buffer.view(), "5");
    EXPECT_EQ(buffer.length(), 1UL);
}

TEST_F(smallstring_simple_test_fixture, append_int64) {
    int64_t to_add = -1254;
    buffer.push(to_add);
    EXPECT_EQ(buffer.view(), "-1254");
    EXPECT_EQ(buffer.length(), 5UL);
}

TEST_F(smallstring_simple_test_fixture, append_size_t) {
    size_t to_add = 12345678910;
    buffer.push(to_add);
    EXPECT_EQ(buffer.view(), "12345678910");
    EXPECT_EQ(buffer.length(), 11UL);
}

TEST_F(smallstring_simple_test_fixture, clear_means_empty_buffer) {
    buffer.push("this is a thing");
    EXPECT_EQ(buffer.view(), "this is a thing");
    EXPECT_EQ(buffer.length(), 15UL);
    buffer.clear();
    EXPECT_EQ(buffer.view(), "");
    EXPECT_EQ(buffer.length(), 0UL);
}

TEST_F(smallstring_simple_test_fixture, mixed_push_and_pop) {
    EXPECT_EQ(buffer.view(), "");
    buffer.push(15);
    EXPECT_EQ(buffer.view(), "15");
    buffer.push("=");
    EXPECT_EQ(buffer.view(), "15=");
    buffer.push("testsymbol");
    EXPECT_EQ(buffer.view(), "15=testsymbol");
    buffer.push("|");
    EXPECT_EQ(buffer.view(), "15=testsymbol|");
    buffer.pop(3);
    EXPECT_EQ(buffer.view(), "testsymbol|");
    buffer.clear();
    EXPECT_EQ(buffer.view(), "");
}