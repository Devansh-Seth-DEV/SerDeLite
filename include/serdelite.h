/*
 * SerDeLite
 * Copyright (C) 2025 Devansh Seth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */


/**
 * @file serdelite.h
 * @brief The master header file for the SerDeLite serialization library.
 * 
 * Including this file provides access to the entire SerDeLite ecosystem, 
 * including binary serialization, JSON construction, and memory management.
 * 
 * @author Devansh Seth
 * @version 1.0.0
 */

#ifndef SERDELITE_H
#define SERDELITE_H

// Core Metadata & System Detection
#include "serdelite/Version.h"
#include "serdelite/Common.h"

// Base Interfaces
#include "serdelite/Serializable.h"

// Memory & Storage
#include "serdelite/ByteBuffer.h"

// Binary Streaming Logic
#include "serdelite/ByteStream.h"

// JSON Construction & Visualization
#include "serdelite/JsonBuffer.h"
#include "serdelite/JsonStream.h"

/**
 * @mainpage SerDeLite Serialization Library
 * @section intro_sec Introduction
 * 
 * SerDeLite is a lightweight, high-performance C++ serialization library 
 * designed for both binary and JSON formats. It is optimized for systems 
 * where memory control and endianness consistency are critical.
 * 
 * @section features_sec Key Features
 * - `Dual-Mode`: Seamlessly switch between compact Binary and readable JSON.
 * - `Endian-Safe`: Automatic host-to-big-endian conversion.
 * - `Memory-Efficient`: Operates on pre-allocated buffers with zero hidden allocations.
 * - `Extensible`: Simple interface-based system for custom object serialization.
 * 
 * @section usage_sec Quick Start
 * @code
 * #include "serdelite.h"
 * 
 * // Prepare a buffer
 * uint8_t mem[1024];
 * serdelite::ByteBuffer buffer(mem, sizeof(mem));
 * 
 * // Start a stream
 * serdelite::ByteStream stream(buffer);
 * stream.writeUint32(42);
 * @endcode
 */

#endif // SERDELITE_H