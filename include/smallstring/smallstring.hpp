#ifndef _SMALLSTRING_SMALLSTRING_HPP_
#define _SMALLSTRING_SMALLSTRING_HPP_

#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace smallstring {

template <typename RawBufferType = std::vector<char>> class Buffer {
  private:
    std::size_t m_length = 0;
    RawBufferType m_buffer;

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

    void clear() { m_length = 0; }

    void append(const char* ptr, std::size_t sz) {
        ensure_fit(sz);
        std::copy(ptr, ptr + sz, tail());
        m_length += sz;
    }

    void append(const std::string_view& view) {
        append(view.data(), view.length());
    }

    void append(const std::string& str) { append(str.data(), str.length()); }

    std::string_view view() {
        return std::string_view(m_buffer.data(), m_length);
    }

    const std::string_view view() const {
        return std::string_view(m_buffer.data(), m_length);
    }
};

} // namespace smallstring

#endif // _SMALLSTRING_SMALLSTRING_HPP_