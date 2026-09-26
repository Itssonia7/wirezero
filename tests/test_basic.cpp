#include <gtest/gtest.h>
#include "wirezero/wirezero.hpp"
#include <vector>
#include <array>

TEST(BufferTest, WriteAndReadPrimitives) {
    std::array<std::byte, 16> backing_buffer{};
    
    // Write phase
    wirezero::BufferWriter writer(backing_buffer);
    writer.write(int32_t(42));
    writer.write(double(3.14159));
    
    EXPECT_EQ(writer.size(), sizeof(int32_t) + sizeof(double));

    // Read phase
    wirezero::BufferReader reader(backing_buffer);
    int32_t val_int = reader.read<int32_t>();
    double val_double = reader.read<double>();

    EXPECT_EQ(val_int, 42);
    EXPECT_DOUBLE_EQ(val_double, 3.14159);
}

TEST(BufferTest, OutOfBoundsWritesThrow) {
    std::array<std::byte, 2> backing_buffer{};
    wirezero::BufferWriter writer(backing_buffer);

    // Trying to write a 4-byte integer into a 2-byte buffer should throw
    EXPECT_THROW(writer.write(int32_t(100)), std::out_of_range);
}

TEST(BufferTest, OutOfBoundsReadsThrow) {
    std::array<std::byte, 2> backing_buffer{};
    wirezero::BufferWriter writer(backing_buffer);
    writer.write(int16_t(500));

    wirezero::BufferReader reader(backing_buffer);
    // Reading 2 bytes is fine
    EXPECT_EQ(reader.read<int16_t>(), 500);

    // Reading another byte should throw out_of_range
    EXPECT_THROW(reader.read<int8_t>(), std::out_of_range);
}