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

class Player : public ByteSerializable {
public:
    uint32_t id;
    float health;
    char name[32];

    Player(uint32_t _id = 0,
           float _health = 0.0f,
           const char* _name = "") 
        : id(_id), health(_health) 
    {
        // Safely copy string and ensure null termination
        strncpy(name, _name, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }

    // This tells the library how many bytes this object needs
    size_t byteSize() const override {
        // 4 (id) + 4 (health) + 2 (string length prefix) + actual string length
        return sizeof(id) +
               sizeof(health) +
               sizeof(uint16_t) + strlen(name);
    }

    bool toByteStream(ByteStream& stream) const override {
        return stream.writeUint32(id) && 
               stream.writeFloat(health) && 
               stream.writeString(name);
    }

    bool fromByteStream(ByteStream& stream) override {
        return stream.readUint32(id) && 
               stream.readFloat(health) && 
               stream.readString(name, sizeof(name));
    }
};

int main() {
    uint8_t rawMemory[128];
    ByteBuffer buffer(rawMemory, sizeof(rawMemory));
    ByteStream stream(buffer);

    // Prepare Data
    Player p1(101, 95.5f, "Hero");

    // Write with Header
    printf("Writing player data...\n");
    stream.writeLibraryHeader();
    
    bool success = stream.writeObject(p1);
    if (!success) printf("Failed to write player into stream\n");
    else printf("Player written into stream\n");

    // Reset and Read
    stream.resetReadCursor();
    
    // early exit if not a verified SerDeLite stream
    if (!stream.verifyLibraryHeader()) {
        printf("Stream is not compatible\n");
        return 0;
    }

    buffer.dump();

    Player p2;
    success = stream.readObject(p2);
    if (!success) printf("Failed to read player from stream\n");
    else {
        printf("\n--- Loaded Data ---\n");
        printf("ID: %u\nHealth: %.1f\nName: %s\n", 
                p2.id, p2.health, p2.name);
    }

    return 0;
}