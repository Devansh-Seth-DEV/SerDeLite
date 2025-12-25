#ifndef SERDELITE_COMMON_H
#define SERDELITE_COMMON_H

#include <stdint.h>

namespace serdelite {

/**
 * @name System & Bit Utilities
 * Fundamental types and static functions used for platform detection and 
 * low-level bit manipulation.
 * @{
 */

/**
 * @enum Endian
 * @brief Represents the byte order of a system or data stream.
 * 
 * `Little`: Least significant byte is stored at the lowest address.
 * 
 * `Big`: Most significant byte is stored at the lowest address (Network Byte Order).
 */
enum class Endian { Little, Big };


/**
 * @brief Detects the current CPU architecture's endianness at runtime.
 * 
 * This function performs a runtime check by inspecting the memory layout 
 * of a 32-bit integer.
 * 
 * @return The detected @ref Endian order of the host system.
 * @note This is used by ByteStream to determine if byte-swapping is 
 *       necessary during serialization.
 */
static inline Endian getSystemEndianness() {
    uint32_t num = 1;
    
    // Get the address of 'num'
    // reinterpret_cast tells the compiler:
    // "Treat this address as a pointer to a byte"
    uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&num);
    
    // Compare the first byte
    return (*bytePtr == 1) ? Endian::Little : Endian::Big;
}


/**
 * @brief Interprets a raw unsigned bit pattern as a signed integer using sign extension.
 * 
 * This utility is critical for correctly reconstructing signed integers from
 * variable-width bit patterns. It manually applies Two's Complement sign 
 * extension if the sign bit of the source number is set.
 * 
 * @param num The raw unsigned value read from the stream.
 * @param bitSize The bit-width of the original type (e.g., 8, 16, 32, 64).
 * @param[out] dest Reference where the signed 64-bit result will be stored.
 * @return true if the bitSize is valid (1-64), false otherwise.
 * 
 * @note This is an internal helper that ensures negative numbers are 
 * preserved across different architecture widths.
 */
static inline bool interpretAsSigned(uint64_t num, uint8_t bitSize, int64_t& dest) {
    if (bitSize == 0 || bitSize > 64) return false;

    const uint64_t signBit = 1ULL << (bitSize - 1);
    const uint64_t valueMask = (bitSize == 64)
                                ? 0
                                : ~((1ULL << bitSize) - 1);

    dest = static_cast<int64_t>(num);

    if (num & signBit) {
        dest |= static_cast<int64_t>(valueMask);
    }

    return true;
}

/** @} */
    
} // namespace serdelite

#endif