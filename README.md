<h1 align="center">
  <img src="serdelite_logo.png" alt="SerDeLite Logo" width="200">
</h1>

<p align="center">
  <img src="https://img.shields.io/github/v/tag/Devansh-Seth-DEV/SerDeLite?label=version&color=blue" alt="Version">
  <img src="https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg" alt="Platform">
  <img src="https://img.shields.io/badge/license-GPL--3.0-orange.svg" alt="License">
  <img src="https://img.shields.io/github/downloads/Devansh-Seth-DEV/SerDeLite/total?color=brightgreen&logo=github" alt="Downloads">
  <img src="https://img.shields.io/github/stars/Devansh-Seth-DEV/SerDeLite?style=flat&logo=github&color=yellow", alt="Stars")
</p>


**SerDeLite** is a lightweight, high-performance C++ serialization library designed for deterministic environments. It provides a unified framework for converting C++ objects into compact **Binary** formats and human-readable **JSON** with zero external dependencies.

SerDeLite is engineered for developers who require absolute control over memory and performance. 


## ✨ Key Features
- **🚀 Zero Dynamic Allocation:** Designed for high-performance and embedded systems. SerDeLite operates entirely on pre-allocated buffers, meaning no new or malloc calls during serialization.
  
- **🔄 Dual-Protocol Support:** Seamlessly switch between compact Binary (for performance) and human-readable JSON (for debugging/config) using the same class structure.

- **📦 Header-Only Friendly:** Minimal dependencies and a lightweight footprint, making it easy to integrate into existing C++11 (or newer) projects.

- **🛠️ Simple Interface:** Turn any class into a serializable object by simply inheriting from JsonSerializable or ByteSerializable and implementing one or two methods.

- **📏 Automatic Memory Management:** The ByteBuffer and JsonBuffer systems prevent buffer overflows and ensure memory safety during stream operations.

- **🎨 Pretty Printing:** Built-in support for "Pretty JSON" formatting, making it easy to generate logs and configuration files that humans can actually read.

## 📥 Direct Downloads
The latest stable binaries and header bundles are available for manual integration.

* **[Download Latest Release](https://github.com/Devansh-Seth-DEV/SerDeLite/releases/latest)**
    * *Includes: `libserdelite.a` (UCRT64/x64) and full `include/` directory.*


## 🛠️ Technical Specifications
SerDeLite provides native support for a wide range of primitive types, ensuring consistent serialization across different architectures.

### Supported Data Types
| Category | Types Supported | Stream Methods |
| :--- | :--- | :--- |
| **Integers** | `uint8`, `int8`, `uint16`, `int16`, `uint32`, `int32`, `uint64`, `int64` | `writeUintXX`, `readUintXX` |
| **Floating Point** | `float` (32-bit), `double` (64-bit) | `writeFloat`, `writeDouble` |
| **Booleans** | `bool` (serialized as 1-byte) | `writeBool`, `readBool` |
| **Strings** | UTF-8 / ASCII null-terminated | `writeString`, `readString` |
| **Custom Objects** | Classes implementing `ByteSerializable` or `JsonSerializable` | `writeObject` |


## 🔧 Architecture Overview
The library is divided into four distinct layers to ensure separation of concerns:

1.  **Physical Layer (`ByteBuffer`):** Direct management of the raw memory storage.
2.  **Logic Layer (`ByteStream` / `JsonStream`):** Implementation of serialization protocols.
3.  **Interface Layer (`Serializable`):** The contract used by custom classes to enable streaming.
4.  **Presentation Layer (`JsonBuffer`):** Tools for visualizing data in human-readable formats.


## 📦 Installation & Deployment

### Windows (MinGW/UCRT64)
Pre-compiled binaries for Windows x64 are available for immediate use.

1. **Download** the `libserdelite.a` from the [Latest Release](https://github.com/Devansh-Seth-DEV/SerDeLite/releases).
2. **Add** the `include` folder to your project's include path.
3. **Link** the library using the following compiler flags:
   ```bash
   -L./bin -lserdelite
    ```

#### 💻 Full Compilation Example
To compile a project using the pre-compiled SerDeLite library, use the following g++ command structure:
```bash
g++ main.cpp -o my_app.exe -I./include -L./bin -lserdelite
```
Breakdown of Flags:
- **main.cpp**: Your application source code.
- **-I./include**: Tells the compiler where to find the serdelite.h header file.
- **-L./bin**: Tells the linker where the libserdelite.a file is stored.
- **-lserdelite**: Links the actual library (Note: the lib prefix and .a extension are omitted here).


### Build from Source
If a custom build is required, the library can be compiled directly from the source code using the following commands:

#### 1. Compile source files into object files
```bash
g++ -c -I./include src/serdelite/*.cpp
```

#### 2. Bundle object files into a static library
```bash
ar rcs bin/libserdelite.a *.o
```

---

## 🚀 Getting Started

### 1. Define a Serializable Object
Inherit from `ByteSerializable` and `JsonSerializable` to enable dual-format support.

```cpp
#include <serdelite.h>
using namespace serdelite;

class Player : public ByteSerializable,
               public JsonSerializable {
public:
    uint32_t id;
    float health;

    Player(uint32_t _id=0, float _health=0)
      : id(_id), health(_health) {}

    // Binary logic
    bool toByteStream(ByteStream& s) const override {
        return s.writeUint32(id) &&
               s.writeFloat(health);
    }

    bool fromByteStream(ByteStream& s) override {
        return s.readUint32(id) && 
               s.readFloat(health);
    }

    size_t byteSize() const override {
      return sizeof(id) + sizeof(health);
    }

protected:
    // JSON logic
    bool serializeToJson(JsonStream& s) const override {
        return s.writeUint32("id", id) &&
               s.writeFloat("health", health);
    }
};
```

### 2. Binary Serialization
```cpp
uint8_t mem[128];
ByteBuffer buffer(mem, sizeof(mem));
ByteStream stream(buffer);

// Write SerDeLite header
stream.writeLibraryHeader();

Player p(101, 95.5f);
bool success = stream.writeObject(p);
if (!success) printf("Failed to serialize Player!\n");
```

### 3. Binary Deserialization
```cpp
// Reset the read cursor first
stream.resetReadCursor();

if (stream.verifyLibraryHeader()) {
   buffer.dump();

   Player p2;

   // Read the previously written player object(p) and store it into p2
   bool success = stream.readObject(p2);
   if (!success) printf("Failed to write player p into player p2");
   else {
      printf("ID: %u\n", p2.id);
      printf("Health: %.1f\n", p2.health);
   }
}
```


### 3. JSON Export & Visualization
```cpp
// using the same buffer to write json
buffer.erase(); // delete all data from buffer
JsonStream jStream(buffer);

success = p.toJson(jStream);
if (!success) printf("Failed to serialize player to Json!");
else jStream.getJson().printPretty();
```