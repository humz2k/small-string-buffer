#ifndef _SMALLSTRING_SMALLSTRING_HPP_
#define _SMALLSTRING_SMALLSTRING_HPP_

#include <cstdint>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace smallstring {

template <typename RawBufferType = std::vector<char>> class Buffer {
  private:
    std::size_t m_length = 0;
    RawBufferType m_buffer;

    template <typename T> int num_digits(T number) {
        int digits = 0;
        if (number < 0)
            digits = 1;
        while (number) {
            number /= 10;
            digits++;
        }
        return digits;
    }

  public:
    Buffer(std::size_t capacity = 256) : m_buffer(capacity) {}
    Buffer(Buffer&& other)
        : m_length(other.m_length), m_buffer(std::move(other.m_buffer)) {
        other.m_length = 0;
    }
    Buffer& operator=(Buffer&& other) {
        m_length = other.m_length;
        other.m_length = 0;
        m_buffer = std::move(other.m_buffer);
        return *this;
    }

    // deleted copy constructors
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    std::size_t capacity() const { return m_buffer.size(); }

    std::size_t length() const { return m_length; }

    std::size_t remaining() const { return capacity() - length(); }

    void drop_memory() {
        m_buffer.clear();
        m_buffer.shrink_to_fit();
        m_length = 0;
    }

    void ensure_fit(const std::size_t to_add) {
        const std::size_t new_capacity = m_length + to_add;
        if (new_capacity > capacity()) {
            m_buffer.resize(new_capacity);
        }
    }

    void ensure_pad(const std::size_t pad) { ensure_fit(pad); }

    char* head() { return m_buffer.data(); }
    const char* head() const { return m_buffer.data(); }

    char* tail() { return m_buffer.data() + m_length; }
    const char* tail() const { return m_buffer.data() + m_length; }

    char* begin() { return head(); }
    const char* begin() const { return head(); }

    char* end() { return tail(); }
    const char* end() const { return tail(); }

    void clear() { m_length = 0; }

    void push(const char* ptr, std::size_t sz) {
        ensure_fit(sz);
        std::copy(ptr, ptr + sz, tail());
        m_length += sz;
    }

    template <std::size_t N> void push(const char (&ptr)[N]) {
        push(ptr, N - 1);
    }

    void push(std::string_view view) { push(view.data(), view.length()); }

    void push(const std::string& str) { push(str.data(), str.length()); }

    void push(int number) {
        const int ndigits = num_digits(number);
        ensure_fit(ndigits);
        m_length += ndigits;
        char* start = tail();
        start--;
        const bool negative = number < 0;
        number = std::abs(number);
        while (number) {
            char digit = (number % 10) + '0';
            *start-- = digit;
            number /= 10;
        }
        if (negative)
            *start = '-';
    }

    void push(int64_t number) {
        const int ndigits = num_digits(number);
        ensure_fit(ndigits);
        m_length += ndigits;
        char* start = tail();
        start--;
        const bool negative = number < 0;
        number = std::abs(number);
        while (number) {
            char digit = (number % 10) + '0';
            *start-- = digit;
            number /= 10;
        }
        if (negative)
            *start = '-';
    }

    void push(size_t number) {
        const int ndigits = num_digits(number);
        ensure_fit(ndigits);
        m_length += ndigits;
        char* start = tail();
        start--;
        while (number) {
            char digit = (number % 10) + '0';
            *start-- = digit;
            number /= 10;
        }
    }

    void pop(const std::size_t n) {
        if (n >= m_length) {
            m_length = 0;
            return;
        }
        std::copy(head() + n, tail(), head());
        m_length = m_length - n;
    }

    std::string_view view() {
        return std::string_view(m_buffer.data(), m_length);
    }

    const std::string_view view() const {
        return std::string_view(m_buffer.data(), m_length);
    }

    std::size_t find(const char* str) const { return view().find(str); }

    std::size_t find(const std::string& str) const { return view().find(str); }
};

} // namespace smallstring

#endif // _SMALLSTRING_SMALLSTRING_HPP_