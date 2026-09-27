#include <gtest/gtest.h>
#include "wirezero/wirezero.hpp"
#include <vector>
#include <array>
#include <string>

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

TEST(PrimitiveSerializerTest, FreeFunctionsRoundtrip) {
    std::array<std::byte, 32> buffer{};
    wirezero::BufferWriter writer(buffer);

    int32_t original_i32 = -123456;
    uint64_t original_u64 = 9876543210ULL;
    float original_f32 = 42.42f;
    double original_f64 = 3.141592653589793;

    // Pack using template free functions
    wirezero::serialize(writer, original_i32);
    wirezero::serialize(writer, original_u64);
    wirezero::serialize(writer, original_f32);
    wirezero::serialize(writer, original_f64);

    // Unpack using template free functions
    wirezero::BufferReader reader(buffer);
    auto deserialized_i32 = wirezero::deserialize<int32_t>(reader);
    auto deserialized_u64 = wirezero::deserialize<uint64_t>(reader);
    auto deserialized_f32 = wirezero::deserialize<float>(reader);
    auto deserialized_f64 = wirezero::deserialize<double>(reader);

    EXPECT_EQ(original_i32, deserialized_i32);
    EXPECT_EQ(original_u64, deserialized_u64);
    EXPECT_FLOAT_EQ(original_f32, deserialized_f32);
    EXPECT_DOUBLE_EQ(original_f64, deserialized_f64);
}

TEST(StringSerializerTest, Roundtrip) {
    std::array<std::byte, 128> buffer{};
    wirezero::BufferWriter writer(buffer);

    std::string original = "WireZero CAD/PLM Binary Protocol Engine";
    wirezero::serialize(writer, std::string_view(original));

    wirezero::BufferReader reader(buffer);
    std::string deserialized = wirezero::deserialize_string(reader);

    EXPECT_EQ(original, deserialized);
}

TEST(VectorSerializerTest, Roundtrip) {
    std::array<std::byte, 256> buffer{};
    wirezero::BufferWriter writer(buffer);

    std::vector<int32_t> original = {10, 20, 30, 40, 50, -999};
    wirezero::serialize(writer, original);

    wirezero::BufferReader reader(buffer);
    std::vector<int32_t> deserialized = wirezero::deserialize_vector<int32_t>(reader);

    EXPECT_EQ(original, deserialized);
}

// Define a sample CAD/PLM data struct
struct CADPart {
    int32_t part_id;
    double volume;
    std::string part_name;
};

// Register it with our WireZero reflection macro
WIREZERO_REFLECT_STRUCT(CADPart, obj.part_id, obj.volume, obj.part_name)

TEST(StructReflectionTest, CADPartRoundtrip) {
    std::array<std::byte, 512> buffer{};
    wirezero::BufferWriter writer(buffer);

    CADPart original{84920, 1548.723, "Bracket_Assembly_v2.step"};

    // Serialize struct
    wirezero::serialize(writer, original);

    // Deserialize struct
    wirezero::BufferReader reader(buffer);
    CADPart deserialized = wirezero::deserialize<CADPart>(reader);

    EXPECT_EQ(original.part_id, deserialized.part_id);
    EXPECT_DOUBLE_EQ(original.volume, deserialized.volume);
    EXPECT_EQ(original.part_name, deserialized.part_name);
}