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

class Stats : public JsonSerializable {
public:
    int32_t level;
    int32_t xp;

    Stats(int32_t _level = 1, int32_t _xp = 0)
        : level(_level), xp(_xp) {}

protected:
    bool serializeToJson(JsonStream& s) const override {
        return s.writeInt32("level", level) && s.writeInt32("xp", xp);
    }
};

class NPC : public JsonSerializable {
public:
    char name[32];
    Stats stats; // Nested Object

    NPC(const char* _name, int32_t level, int32_t xp)
        : stats(level, xp) {
        snprintf(name, sizeof(name), "%s", _name);
    }

protected:
    bool serializeToJson(JsonStream& s) const override {
        return s.writeString("name", name) && 
               s.writeObject("stats", stats); // NESTED JSON OBJECT
    }
};

int main() {
    uint8_t mem[256];
    ByteBuffer buf(mem, sizeof(mem));
    JsonStream jStream(buf);

    NPC myNpc("Merchant", 15, 4500);

    bool success = myNpc.toJson(jStream);
    if (!success) printf("Failed to serialize NPC.\n");
    else jStream.getJson().printPretty();

    return 0;
}