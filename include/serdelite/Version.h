/*
 * SerDeLite
 * Copyright (C) 2025 Devansh Seth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */

#ifndef SERDELITE_VERSION_H
#define SERDELITE_VERSION_H

/**
 * @name Library Metadata
 * Compile-time constants and macros used for versioning and stream identification.
 * @{
 */

/** @brief Major version number. Incremented for incompatible API changes. */
#define SERDELITE_VERSION_MAJOR 1

/** @brief Minor version number. Incremented for backward-compatible functionality. */
#define SERDELITE_VERSION_MINOR 1

/** @brief Patch version number. Incremented for backward-compatible bug fixes. */
#define SERDELITE_VERSION_PATCH 0

/**
 * @brief The 4-byte "Magic Number" used to identify SerDeLite binary streams.
 * 
 * This uses the `FourCC` (Four Character Code) technique with the ASCII values 
 * for "SDLV" (0x53444C56). This acts as a file signature at the start of 
 * every valid SerDeLite data stream.
 */
#define SERDELITE_MAGIC 0x53444C56

/** @brief A helper macro to stringify version integers. */
#define SERDELITE_STR_HELPER(x) #x

/** @brief A macro that converts a numeric value into a string literal. */
#define SERDELITE_STR(x) SERDELITE_STR_HELPER(x)

/** @brief The complete version string literal (e.g., "1.0.0"). */
#define SERDELITE_VERSION_STR SERDELITE_STR(SERDELITE_VERSION_MAJOR) "." \
                              SERDELITE_STR(SERDELITE_VERSION_MINOR) "." \
                              SERDELITE_STR(SERDELITE_VERSION_PATCH)

namespace serdelite {
    /**
     * @brief Retrieves the current version of the library as a string.
     * @return A constant character pointer to the semantic version string (e.g., "1.0.0").
     */
    inline const char* getVersion() {
        return SERDELITE_VERSION_STR;
    }
}

/** @} */

#endif