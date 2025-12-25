# SerDeLite 🚀
A lightweight, endian-aware C++ serialization library for Binary and JSON.

## 📦 Installation (Windows/MinGW)
1. Clone the repository.
2. Link the static library in your build system:
   - **Include path:** `-I./include`
   - **Library path:** `-L./bin`
   - **Linker flag:** `-lserdelite`

## 🛠️ Usage Example
```cpp
#include "serdelite.h"

int main() {
    uint8_t mem[256];
    serdelite::ByteBuffer buffer(mem, sizeof(mem));
    serdelite::ByteStream stream(buffer);
    
    stream.writeUint32(12345); // Writes in Big-Endian automatically
    return 0;
}

```