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
        printf("Header Mismatch, Not a valid SerDeLite packet!\n");
        return;
    }

    // LAYER 2: Application Type Identification
    uint8_t type;
    bool success = stream.readUint8(type);
    if (!success) {
        printf("Failed to read the packet type.\n");
        return;
    } else printf("Recieved packet type: %u\n", type);

    printf("Reading packet data...\n");

    switch (type) {
    case MSG_PLAYER_DATA: {
        PlayerData p;
        success = stream.readObject(p);
        if (!success) printf("Failed to read player data.\n");
        else {
            printf("Successfully read player's data.\n\n");
            printf("Player:\n");
            printf("id: %u\n", p.id);
        }
    }
    break;

    case MSG_CHAT: {
        char chatBuffer[64];
        success = stream.readString(chatBuffer,
                                    sizeof(chatBuffer));
        
        if (!success) printf("Failed to read the chat message.\n");
        else {
            printf("Succesfully read chat message.\n\n");
            printf("Message: \"%s\"\n", chatBuffer);
        }
    }
    break;

    default:
        printf("Unknown packet type.\n");
    }
}

int main() {
    uint8_t mem[128];
    ByteBuffer buffer(mem, sizeof(mem));
    ByteStream stream(buffer);
    
    // Serializing player
    stream.writeLibraryHeader();
    stream.writeUint8(MSG_PLAYER_DATA);
    PlayerData p1(42);
    stream.writeObject(p1);
    buffer.dump();  // display the raw-memory state

    // --- TEST 1: Sending Player Data ---
    printf("\n--- SENDING PLAYER ---\n");
    stream.resetReadCursor();
    processIncomingData(buffer);

    // Serializing chat message
    buffer.erase();
    stream.writeLibraryHeader();
    stream.writeUint8(MSG_CHAT);
    stream.writeString("Hello World!");
    buffer.dump();   // display the raw-memory state

    // --- TEST 2: Sending Chat ---
    printf("\n--- SENDING CHAT ---\n");
    stream.resetReadCursor();
    processIncomingData(buffer);

    return 0;
}