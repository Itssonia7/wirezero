#pragma once

#include "buffer.hpp"
#include <type_traits>
#include <string>
#include <vector>
#include <string_view>
#include <tuple>

namespace wirezero {

template <typename T>
concept TrivialSerializable = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

// Forward declarations for general dispatch
template <typename T>
inline void serialize(BufferWriter& writer, const T& value);

template <typename T>
inline T deserialize(BufferReader& reader);

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

// Specialization for std::string deserialization via template dispatch
template <>
inline std::string deserialize<std::string>(BufferReader& reader) {
    return deserialize_string(reader);
}

// -----------------------------------------------------------------------------
// std::vector Serialization
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

// -----------------------------------------------------------------------------
// C++20 Struct Metaprogramming & Reflection Helpers
// -----------------------------------------------------------------------------

/**
 * @brief Serializes a tuple of struct fields recursively using C++17/20 fold expressions.
 */
template <typename... Args>
inline void serialize_tuple(BufferWriter& writer, const std::tuple<Args...>& t) {
    std::apply([&writer](const auto&... args) {
        (serialize(writer, args), ...);
    }, t);
}

/**
 * @brief Deserializes a tuple of struct field references recursively using fold expressions.
 */
template <typename... Args>
inline void deserialize_tuple(BufferReader& reader, std::tuple<Args&...>& t) {
    std::apply([&reader](auto&... args) {
        ((args = deserialize<std::remove_cvref_t<decltype(args)>>(reader)), ...);
    }, t);
}

} // namespace wirezero

/**
 * @brief Macro to automatically generate serialize and deserialize functions for user structs.
 * Example usage:
 *   struct Part { int id; std::string name; };
 *   WIREZERO_REFLECT_STRUCT(Part, id, name)
 */
#define WIREZERO_REFLECT_STRUCT(StructName, ...) \
    inline void serialize(wirezero::BufferWriter& writer, const StructName& obj) { \
        auto t = std::tie(__VA_ARGS__); \
        wirezero::serialize_tuple(writer, t); \
    } \
    inline StructName deserialize_##StructName(wirezero::BufferReader& reader) { \
        StructName obj{}; \
        auto t = std::tie(__VA_ARGS__); \
        wirezero::deserialize_tuple(reader, t); \
        return obj; \
    } \
    template <> \
    inline StructName wirezero::deserialize<StructName>(wirezero::BufferReader& reader) { \
        return deserialize_##StructName(reader); \
    }