#pragma once

#include "buffer.hpp"
#include <type_traits>

namespace wirezero {

// Concept to verify a type is a serializable primitive/trivial type
template <typename T>
concept TrivialSerializable = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

/**
 * @brief Serialize a primitive value into a BufferWriter.
 */
template <TrivialSerializable T>
inline void serialize(BufferWriter& writer, const T& value) {
    writer.write(value);
}

/**
 * @brief Deserialize a primitive value from a BufferReader.
 */
template <TrivialSerializable T>
inline T deserialize(BufferReader& reader) {
    return reader.read<T>();
}

} // namespace wirezero