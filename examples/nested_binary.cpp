/*
 * SerDeLite
 * Copyright (C) 2025 Devansh Seth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>

#include <serdelite.h>

using namespace serdelite;

class Item : public ByteSerializable {
public:
    uint16_t id;
    uint8_t quantity;

    Item(uint16_t _id = 0, uint8_t _qty = 0)
        : id(_id), quantity(_qty) {}

    // 2 bytes (id) + 1 byte (qty) = 3 bytes total
    size_t byteSize() const override {
        return sizeof(id) + sizeof(quantity);
    }

    bool toByteStream(ByteStream& s) const override {
        return s.writeUint16(id) && s.writeUint8(quantity);
    }

    bool fromByteStream(ByteStream& s) override {
        return s.readUint16(id) && s.readUint8(quantity);
    }
};


class Inventory : public ByteSerializable {
public:
    uint32_t ownerId;
    Item slotA; // First nested object
    Item slotB; // Second nested object

    Inventory(uint32_t _ownerId = 0): ownerId(_ownerId) {}

    Inventory(uint32_t _owner,
              Item _slotA,
              Item _slotB)
        : ownerId(_owner), slotA(_slotA), slotB(_slotB) {}

    size_t byteSize() const override {
        return sizeof(ownerId) +
               slotA.byteSize() +
               slotB.byteSize();
    }

    bool toByteStream(ByteStream& s) const override {
        // Write the owner ID, then the two items
        return s.writeUint32(ownerId) && 
               s.writeObject(slotA) && 
               s.writeObject(slotB);
    }

    bool fromByteStream(ByteStream& s) override {
        // Read in same order in which they are written
        return s.readUint32(ownerId) && 
               s.readObject(slotA) && 
               s.readObject(slotB);
    }
};

int main() {
    uint8_t rawMemory[128];
    ByteBuffer buffer(rawMemory, sizeof(rawMemory));
    ByteStream stream(buffer);

    // Setup Nested Data
    Inventory inv(500,          // Owner ID 500
                  Item(10, 5),  // Item ID 10, Qty 5
                  Item(20, 1)); // Item ID 20, Qty 1

    // Serialize
    printf("Serializing Nested Inventory...\n");
    stream.writeLibraryHeader();

    bool success = stream.writeObject(inv);
    if (!success) printf("Failed to serialize inventory.\n");
    else printf("Inventory serialized.\n");

    buffer.dump();

    stream.resetReadCursor();

    // early exit if not a verified SerDeLite stream
    if (!stream.verifyLibraryHeader()) {
        printf("Stream is not compatible.\n");
        return 0;
    }
    
    Inventory loadedInv;

    success = stream.readObject(loadedInv);
    if (!success)
        printf("Failed to read inventory from stream.\n");
    else {
        printf("\n--- Loaded Data ---\n");
        printf("Owner: %u\n", loadedInv.ownerId);
        
        printf("Slot A: ID %u, Qty %u\n",
                loadedInv.slotA.id,
                loadedInv.slotA.quantity);
        
        printf("Slot B: ID %u, Qty %u\n",
                loadedInv.slotB.id,
                loadedInv.slotB.quantity);
    }

    return 0;
}