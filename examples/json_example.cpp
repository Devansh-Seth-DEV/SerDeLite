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

class Config : public JsonSerializable {
public:
    int32_t sensitivity;
    bool fullscreen;

    Config(int32_t sensti, bool fullscr)
        : sensitivity(sensti), fullscreen(fullscr) {}

protected:
    bool serializeToJson(JsonStream& stream) const override {
        return stream.writeInt32("sensitivity", sensitivity) && 
               stream.writeBool("fullscreen", fullscreen);
    }
};

int main() {
    uint8_t rawMemory[256];
    ByteBuffer buffer(rawMemory, sizeof(rawMemory));
    JsonStream jStream(buffer);

    // Setup Object
    Config myConfig(85, true);

    // Serialize
    bool success = myConfig.toJson(jStream);
    if (!success) printf("Failed to serialize Config object to Json!\n");
    else {
        // Get the result and print
        JsonBuffer jBuffer = jStream.getJson();
        
        printf("Raw JSON: %.*s\n\n",
                (int)jBuffer.length,
                jBuffer.data);
        
        printf("Pretty JSON:\n");
        jBuffer.printPretty(); // 2 spaces indent (default)
    }

    return 0;
}