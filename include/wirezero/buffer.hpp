#pragma once

#include <span>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace wirezero {

/**
 * @brief A safe reader view over a contiguous block of binary data.
 * Performs strict bounds checking to prevent buffer over-reads.
 */
class BufferReader {
public:
    // Construct from a standard byte span or raw pointer + size
    constexpr explicit BufferReader(std::span<const std::byte> data) noexcept
        : data_(data), offset_(0) {}

    constexpr BufferReader(const uint8_t* ptr, size_t size) noexcept
        : data_(reinterpret_cast<const std::byte*>(ptr), size), offset_(0) {}

    // Read a trivial type (primitives like int32_t, float, etc.) using zero-copy memcpy
    template <typename T>
    requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    T read() {
        if (offset_ + sizeof(T) > data_.size()) {
            throw std::out_of_range("BufferReader: read out of bounds");
        }
        T value;
        std::memcpy(&value, data_.data() + offset_, sizeof(T));
        offset_ += sizeof(T);
        return value;
    }

    // Peek a value without advancing the read offset
    template <typename T>
    requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    T peek() const {
        if (offset_ + sizeof(T) > data_.size()) {
            throw std::out_of_range("BufferReader: peek out of bounds");
        }
        T value;
        std::memcpy(&value, data_.data() + offset_, sizeof(T));
        return value;
    }

    // Skip N bytes
    constexpr void advance(size_t n) {
        if (offset_ + n > data_.size()) {
            throw std::out_of_range("BufferReader: advance out of bounds");
        }
        offset_ += n;
    }

    [[nodiscard]] constexpr size_t remaining() const noexcept {
        return data_.size() - offset_;
    }

    [[nodiscard]] constexpr size_t offset() const noexcept {
        return offset_;
    }

private:
    std::span<const std::byte> data_;
    size_t offset_;
};

/**
 * @brief A safe writer view over a mutable block of binary data.
 * Performs strict bounds checking to prevent buffer overflows.
 */
class BufferWriter {
public:
    constexpr explicit BufferWriter(std::span<std::byte> data) noexcept
        : data_(data), offset_(0) {}

    constexpr BufferWriter(uint8_t* ptr, size_t size) noexcept
        : data_(reinterpret_cast<std::byte*>(ptr), size), offset_(0) {}

    // Write a trivial type directly into the buffer via memcpy
    template <typename T>
    requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    void write(const T& value) {
        if (offset_ + sizeof(T) > data_.size()) {
            throw std::out_of_range("BufferWriter: write out of bounds");
        }
        std::memcpy(data_.data() + offset_, &value, sizeof(T));
        offset_ += sizeof(T);
    }

    [[nodiscard]] constexpr size_t capacity() const noexcept {
        return data_.size();
    }

    [[nodiscard]] constexpr size_t size() const noexcept {
        return offset_;
    }

    [[nodiscard]] constexpr size_t remaining() const noexcept {
        return data_.size() - offset_;
    }

private:
    std::span<std::byte> data_;
    size_t offset_;
};

} // namespace wirezero