#pragma once

#include "buffer.hpp"
#include <type_traits>
#include <string>
#include <vector>
#include <string_view>

namespace wirezero {

template <typename T>
concept TrivialSerializable = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

// -----------------------------------------------------------------------------
// Primitives
// -----------------------------------------------------------------------------
template <TrivialSerializable T>
inline void serialize(BufferWriter& writer, const T& value) {
    writer.write(value);
}

template <TrivialSerializable T>
inline T deserialize(BufferReader& reader) {
    return reader.read<T>();
}

// -----------------------------------------------------------------------------
// std::string & std::string_view Serialization
// -----------------------------------------------------------------------------
inline void serialize(BufferWriter& writer, std::string_view str) {
    auto len = static_cast<uint32_t>(str.size());
    writer.write(len);
    if (len > 0) {
        writer.write_bytes(reinterpret_cast<const std::byte*>(str.data()), len);
    }
}

inline std::string deserialize_string(BufferReader& reader) {
    auto len = reader.read<uint32_t>();
    if (len == 0) {
        return {};
    }
    auto span_bytes = reader.read_span(len);
    return std::string(reinterpret_cast<const char*>(span_bytes.data()), len);
}

// -----------------------------------------------------------------------------
// std::vector Serialization (for trivial types)
// -----------------------------------------------------------------------------
template <typename T>
requires TrivialSerializable<T>
inline void serialize(BufferWriter& writer, const std::vector<T>& vec) {
    auto count = static_cast<uint32_t>(vec.size());
    writer.write(count);
    if (count > 0) {
        writer.write_bytes(reinterpret_cast<const std::byte*>(vec.data()), count * sizeof(T));
    }
}

template <typename T>
requires TrivialSerializable<T>
inline std::vector<T> deserialize_vector(BufferReader& reader) {
    auto count = reader.read<uint32_t>();
    if (count == 0) {
        return {};
    }
    std::vector<T> vec(count);
    auto span_bytes = reader.read_span(count * sizeof(T));
    std::memcpy(vec.data(), span_bytes.data(), count * sizeof(T));
    return vec;
}

} // namespace wirezero