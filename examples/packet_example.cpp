/*
 * SerDeLite
 * Copyright (C) 2025 Devansh Seth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */

#include <stdio.h>
#include <serdelite.h>

using namespace serdelite;

// Define the Packet Types
enum PacketType : uint8_t {
    MSG_PLAYER_DATA = 1,
    MSG_CHAT        = 2
};

// A simple data class
class PlayerData : public ByteSerializable {
public:
    uint32_t id;
    PlayerData(uint32_t _id = 0) : id(_id) {}
    size_t byteSize() const override { return sizeof(id); }
    bool toByteStream(ByteStream& s) const override { return s.writeUint32(id); }
    bool fromByteStream(ByteStream& s) override { return s.readUint32(id); }
};

// --- RECEIVER LOGIC ---
void processIncomingData(ByteBuffer& buffer) {
    ByteStream stream(buffer);
    
    // LAYER 1: Library Verification
    if (!stream.verifyLibraryHeader()) {
        printf("[Error] Header Mismatch, Not a valid SerDeLite packet!\n");
        return;
    }

    // LAYER 2: Application Type Identification
    uint8_t type;
    if (!stream.readUint8(type)) return;

    if (type == MSG_PLAYER_DATA) {
        PlayerData p;
        if (stream.readObject(p)) {
            printf("[Success] Received Player ID: %u\n", p.id);
        }
    } else if (type == MSG_CHAT) {
        char chatBuffer[64];
        // SerDeLite's readString handles the length prefix automatically
        if (stream.readString(chatBuffer, sizeof(chatBuffer))) {
            printf("[Success] Received Chat Message: \"%s\"\n", chatBuffer);
        }
    } else {
        printf("[Warning] Unknown Packet Type: %u\n", type);
    }
}

int main() {
    uint8_t mem[128];
    ByteBuffer buffer(mem, sizeof(mem));
    ByteStream stream(buffer);

    // --- TEST 1: Sending Player Data ---
    printf("--- SENDING PLAYER ---\n");
    stream.writeLibraryHeader();
    stream.writeUint8(MSG_PLAYER_DATA);
    PlayerData p1(42);
    stream.writeObject(p1);
    buffer.dump();
    stream.resetReadCursor();
    processIncomingData(buffer);

    // --- TEST 2: Sending Chat (Clear and Re-use Buffer) ---
    buffer.erase();
    stream.resetReadCursor();

    printf("\n--- SENDING CHAT ---\n");
    stream.writeLibraryHeader();
    stream.writeUint8(MSG_CHAT);
    stream.writeString("Hello World!");
    buffer.dump();
    stream.resetReadCursor();
    processIncomingData(buffer);

    return 0;
}