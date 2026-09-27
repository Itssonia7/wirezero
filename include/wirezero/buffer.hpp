#pragma once

#include <span>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string_view>

namespace wirezero {

class BufferReader {
public:
    constexpr explicit BufferReader(std::span<const std::byte> data) noexcept
        : data_(data), offset_(0) {}

    constexpr BufferReader(const uint8_t* ptr, size_t size) noexcept
        : data_(reinterpret_cast<const std::byte*>(ptr), size), offset_(0) {}

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

    // Read a raw block of bytes (useful for strings and vectors)
    void read_bytes(std::byte* dest, size_t size) {
        if (offset_ + size > data_.size()) {
            throw std::out_of_range("BufferReader: read_bytes out of bounds");
        }
        std::memcpy(dest, data_.data() + offset_, size);
        offset_ += size;
    }

    // Get a zero-copy non-owning view directly into the buffer memory at current offset!
    [[nodiscard]] std::span<const std::byte> read_span(size_t size) {
        if (offset_ + size > data_.size()) {
            throw std::out_of_range("BufferReader: read_span out of bounds");
        }
        auto span_view = data_.subspan(offset_, size);
        offset_ += size;
        return span_view;
    }

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

class BufferWriter {
public:
    constexpr explicit BufferWriter(std::span<std::byte> data) noexcept
        : data_(data), offset_(0) {}

    constexpr BufferWriter(uint8_t* ptr, size_t size) noexcept
        : data_(reinterpret_cast<std::byte*>(ptr), size), offset_(0) {}

    template <typename T>
    requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    void write(const T& value) {
        if (offset_ + sizeof(T) > data_.size()) {
            throw std::out_of_range("BufferWriter: write out of bounds");
        }
        std::memcpy(data_.data() + offset_, &value, sizeof(T));
        offset_ += sizeof(T);
    }

    // Write a raw block of bytes
    void write_bytes(const std::byte* src, size_t size) {
        if (offset_ + size > data_.size()) {
            throw std::out_of_range("BufferWriter: write_bytes out of bounds");
        }
        std::memcpy(data_.data() + offset_, src, size);
        offset_ += size;
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