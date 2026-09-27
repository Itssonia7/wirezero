




# WireZero 🚀

**WireZero** is a high-performance, zero-copy binary protocol serializer and deserializer library built from scratch in **modern C++20**. It is engineered specifically for performance-critical systems—such as **CAD/PLM data processing** and telemetry engines—where traditional text-based formats like JSON or XML introduce unacceptable parsing overhead and heap allocation bottlenecks.

---

## ✨ Key Features

- **Zero-Copy & Heap-Free Buffers:** Non-owning `BufferReader` and `BufferWriter` abstractions wrapping raw memory segments via `std::span` with zero dynamic allocations.
- **Strict Bounds Safety:** Built-in bounds checking that safely throws `std::out_of_range` to prevent buffer overflows or over-reads.
- **Concept-Constrained Primitives:** Leverages C++20 concepts (`TrivialSerializable`) to ensure type safety for fixed-width primitives (`int32_t`, `uint64_t`, `float`, `double`, etc.).
- **Efficient Variable-Length Streaming:** Length-prefixed serialization and bulk memory copying for `std::string`, `std::string_view`, and `std::vector`.
- **C++20 Reflection Macro:** A lightweight metaprogramming helper (`WIREZERO_REFLECT_STRUCT`) utilizing variadic templates and fold expressions to serialize user-defined structs with zero boilerplate.
- **Production-Grade Testing & Benchmarking:** Automated test suite powered by GoogleTest and built-in high-throughput benchmarks.

---

## 📂 Project Structure

```text
wirezero/
├── CMakeLists.txt         # Top-level CMake configuration (C++20 & GoogleTest)
├── include/
│   └── wirezero/
│       ├── wirezero.hpp   # Main library header
│       ├── buffer.hpp     # Safe BufferReader & BufferWriter views
│       └── serializer.hpp # Primitives, containers, and reflection macros
├── src/
│   └── wirezero.cpp       # Library implementation source
└── tests/
    ├── CMakeLists.txt     # Test suite configuration
    └── test_basic.cpp     # Unit tests and performance benchmarks

```

---

## 🛠️ Getting Started

### Prerequisites

* A modern C++ compiler supporting **C++20** (GCC 11+, Clang 13+, or MSVC 2019+).
* **CMake** (Version 3.21 or higher).

### Building and Running Tests

Clone the repository and compile the project using CMake:

```bash
# Configure the project in Release mode
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the library and test suite
cmake --build build --config Release

# Run automated tests and benchmarks
ctest --test-dir build --output-on-failure

```

---

## 💻 Code Example

```cpp
#include "wirezero/wirezero.hpp"
#include <array>
#include <iostream>

// 1. Define your CAD/PLM data structure
struct CADPart {
    int32_t part_id;
    double volume;
    std::string part_name;
};

// 2. Register fields using the reflection macro
WIREZERO_REFLECT_STRUCT(CADPart, obj.part_id, obj.volume, obj.part_name)

int main() {
    std::array<std::byte, 256> buffer{};
    
    // Serialize into buffer
    wirezero::BufferWriter writer(buffer);
    CADPart original{4291, 1548.723, "Assembly_Bracket_v1.step"};
    wirezero::serialize(writer, original);

    // Deserialize from buffer
    wirezero::BufferReader reader(buffer);
    CADPart deserialized = wirezero::deserialize<CADPart>(reader);

    std::cout << "Successfully parsed Part ID: " << deserialized.part_id << "\n";
    return 0;
}

```

---

## 📊 Performance Benchmark

WireZero bypasses text-parsing overhead by mapping binary structures directly into memory blocks. In local benchmarks, WireZero serializes and deserializes **100,000 complex CAD structs in milliseconds**, making it ideal for real-time CAD model streaming and PLM pipeline logging.

---

## 📜 License

Distributed under the MIT License. See `LICENSE` for more information.




