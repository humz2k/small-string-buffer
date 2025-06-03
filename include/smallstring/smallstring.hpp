/**
 * @file buffer.hpp
 * @brief Small, self-contained string-building buffer.
 *
 * `smallstring::Buffer` is a grow-as-needed, *write-only* buffer that lets you
 * incrementally append character data and integral values without paying the
 * overhead of `std::ostringstream` or repeated `std::string` reallocations.
 *
 * ### Design goals
 * * **Simplicity** – single header, no external deps.
 * * **Allocator awareness** – templated on the allocator type so you can plug
 *   in a custom arena / pool.
 * * **Zero-copy reads** – expose the contents as `std::string_view`.
 * * **No exceptions on push** – the only operations that can throw are the
 *   underlying `std::vector` reallocations when the buffer runs out of space.
 */

#pragma once

#include <algorithm> // std::copy
#include <cmath>     // std::abs
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace smallstring {

/**
 * @class Buffer
 * @brief Lightweight mutable byte buffer specialised for building strings.
 *
 * @tparam Alloc  `std::allocator<char>` by default; any STL-style allocator
 *                that allocates `char` is accepted.
 *
 * Typical usage
 * @code
 *   smallstring::Buffer<> buf;
 *   buf.push("ID=");
 *   buf.push(42);
 *   buf.push('|');
 *   buf.push(price);          // price is an int / long / size_t …
 *   std::cout << buf.view();  // -> "ID=42|1234"
 * @endcode
 *
 * @note  All offsets are tracked in **bytes**, *not* code-points.  The class
 *        is agnostic to encoding; treat it as raw bytes.
 */
template <class Alloc = std::allocator<char>> class Buffer {
  private:
    std::size_t m_length = 0;          ///< Number of bytes currently used
    std::vector<char, Alloc> m_buffer; ///< Backing storage

    /**
     * @brief Return the number of base-10 digits needed to print @p number.
     *
     * Handles negative values (adds one char for the sign) when the type is
     * signed.
     */
    template <typename T,
              typename = std::enable_if_t<std::is_integral<T>::value>>
    std::size_t count_digits(T number) const {
        std::size_t digits = 0;
        if constexpr (std::is_signed<T>::value) {
            if (number < 0)
                digits = static_cast<T>(1);
        }
        while (number) {
            number /= static_cast<T>(10);
            ++digits;
        }
        return digits;
    }

  public:
    /// @brief Construct the buffer with an initial @p capacity (bytes).
    explicit Buffer(std::size_t capacity = 256) : m_buffer(capacity) {}

    Buffer(Buffer&& other) = default;
    Buffer& operator=(Buffer&& other) = default;

    Buffer(const Buffer&) = default;
    Buffer& operator=(const Buffer&) = default;

    /* --------------------------------------------------------------------- */
    /*                              Capacity API                             */
    /* --------------------------------------------------------------------- */

    /// @brief Current capacity in bytes.
    std::size_t capacity() const { return m_buffer.size(); }
    /// @brief Bytes already written.
    std::size_t length() const { return m_length; }
    /// @brief Free space remaining before a resize is required.
    std::size_t remaining() const { return capacity() - length(); }

    /// @brief Discard all memory.
    void drop_memory() {
        m_buffer.clear();
        m_buffer.shrink_to_fit();
        m_length = 0;
    }

    /// @brief Ensure another @p to_add bytes can be appended without resize.
    void ensure_fit(const std::size_t to_add) {
        const std::size_t new_capacity = m_length + to_add;
        if (new_capacity > capacity()) {
            m_buffer.resize(new_capacity);
        }
    }

    /// @brief Ensure another @p to_add bytes can be appended without resize.
    void ensure_pad(const std::size_t pad) { ensure_fit(pad); }

    /* --------------------------------------------------------------------- */
    /*                         Raw pointer convenience                       */
    /* --------------------------------------------------------------------- */

    char* head() { return m_buffer.data(); }
    const char* head() const { return m_buffer.data(); }

    char* tail() { return m_buffer.data() + m_length; }
    const char* tail() const { return m_buffer.data() + m_length; }

    char* begin() { return head(); }
    const char* begin() const { return head(); }

    char* end() { return tail(); }
    const char* end() const { return tail(); }

    /// @brief Logical clear – the buffer’s capacity is unchanged.
    void clear() { m_length = 0; }

    /* --------------------------------------------------------------------- */
    /*                            Push operations                            */
    /* --------------------------------------------------------------------- */

    /// @brief Append @p sz bytes starting at @p ptr.
    void push(const char* ptr, std::size_t sz) {
        ensure_fit(sz);
        std::copy(ptr, ptr + sz, tail());
        m_length += sz;
    }

    /// @brief Append a string literal (deduces size at compile time).
    template <std::size_t N> void push(const char (&ptr)[N]) {
        push(ptr, N - 1);
    }

    /// @brief Append the contents of a `std::string_view`.
    void push(std::string_view view) { push(view.data(), view.length()); }

    /// @brief Append the contents of a `std::string`.
    void push(const std::string& str) { push(str.data(), str.length()); }

    /**
     * @brief Append an integral value in base-10 with no allocations.
     *
     * Handles signedness automatically.  The implementation writes digits
     * right-to-left starting at `tail()`.
     */
    template <typename T,
              typename = std::enable_if_t<std::is_integral<T>::value>>
    void push(T number) {
        const auto ndigits = count_digits(number);

        ensure_fit(ndigits);
        m_length += ndigits;

        char* start = tail();
        start--;

        bool negative = false;
        if constexpr (std::is_signed<T>::value) {
            negative = number < 0;
            number = std::abs(number);
        }

        while (number) {
            char digit = (number % 10) + '0';
            *start-- = digit;
            number /= 10;
        }

        if (negative)
            *start = '-';
    }


    /* --------------------------------------------------------------------- */
    /*                            Pop operations                             */
    /* --------------------------------------------------------------------- */

    /**
     * @brief Remove the first @p n bytes, closing the gap by memmoving.
     *
     * When @p n ≥ current length the buffer is simply cleared.
     */
    void pop(const std::size_t n) {
        if (n >= m_length) {
            m_length = 0;
            return;
        }
        std::copy(head() + n, tail(), head());
        m_length = m_length - n;
    }

    /* --------------------------------------------------------------------- */
    /*                              View / find                              */
    /* --------------------------------------------------------------------- */

    /// @brief Return a view of the valid byte range `[head(), tail())`.
    [[nodiscard]] std::string_view view() const { return std::string_view(head(), length()); }

    /// @brief Find overload – transparently forward to `std::string_view::find`.
    std::size_t find(const char* str, std::size_t pos = 0UL) const {
        return view().find(str, pos);
    }

    /// @brief Find overload – transparently forward to `std::string_view::find`.
    std::size_t find(const std::string& str, std::size_t pos = 0UL) const {
        return view().find(str, pos);
    }

    /// @brief Find overload – transparently forward to `std::string_view::find`.
    std::size_t find(std::string_view str, std::size_t pos = 0UL) const {
        return view().find(str, pos);
    }
};

} // namespace smallstring