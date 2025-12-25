/*
 * SerDeLite
 * Copyright (C) 2025 Devansh Seth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */

#ifndef SERDELITE_JSONBUFFER_H
#define SERDELITE_JSONBUFFER_H

#include <stddef.h>
#include <stdio.h>

namespace serdelite {

/**
 * @name JSON Visualization
 * Formats and displays the final JSON string
 * @{
 */

/**
 * @class JsonBuffer
 * @brief A read-only container for JSON string data with visualization capabilities.
 * 
 * JsonBuffer is the primary output type of the JsonStream. It stores a pointer 
 * to the serialized JSON text and provides a powerful `printPretty` utility 
 * to display the JSON in a human-readable, indented format. 
 * 
 * @note This class does not own the memory it points to; it typically points 
 * to the internal memory of the ByteBuffer used during the streaming process.
 * 
 * @sa JsonStream
 * @sa ByteBuffer
 */
class JsonBuffer {
public:
    const char* data;
    size_t length;

    /**
     * @brief Construct a new `JsonBuffer` object.
     * @param _data A pointer to the existing JSON string (must be null-terminated or match len).
     * @param len The length of the string to be managed.
     */
    JsonBuffer(const char* _data, size_t len);

    /**
     * @brief Prints the JSON content to the console with proper indentation and line breaks.
     * 
     * This function parses the raw JSON string and applies formatting to make it
     * human-readable. It handles braces `{}`, brackets `[]`, and commas `,` by
     * inserting newlines and appropriate indentation.
     * 
     * @param tabWidth The number of spaces to use for each indentation level. Default is 2.
     */
    void printPretty(int tabWidth = 2) const;

private:
    void printIndent(int level, int tabWidth) const;
};

/** @} */

} // namespace serdelite

#endif