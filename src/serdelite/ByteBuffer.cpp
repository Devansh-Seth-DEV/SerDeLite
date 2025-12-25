#include "serdelite/ByteBuffer.h"

#include <string.h>
#include <assert.h>
#include <cstdio>

namespace serdelite {

ByteBuffer::ByteBuffer(uint8_t* buffer,
                       size_t bufferCapacity,
                       Endian endianOrder)
    : bytes(buffer),
      capacity(bufferCapacity),
      length(0),
      order(endianOrder)
{
    assert(this->bytes != nullptr &&
           "ByteBuffer requires valid memory");
    assert(bufferCapacity > 0 &&
           "ByteBuffer requires a valid non-zero capacity");

    memset(this->bytes, 0, bufferCapacity);
}

bool ByteBuffer::toString(char* dest,
                          size_t destCapacity) const {
    if (!dest || destCapacity == 0) return false;

    size_t copyLen = (this->length < destCapacity-1)
                     ? this->length
                     : destCapacity-1;

    for (size_t i = 0; i < copyLen; i++) {
        uint8_t byte = this->bytes[i];
        
        // Check if the character is "printable" (ASCII space 32 to tilde 126)
        if (byte >= 32 && byte <= 126) {
            dest[i] = static_cast<char>(byte);
        } else {
            // Placeholder for non-printable binary data
            dest[i] = '.';
        }
    }

    // null-terminate for c-style string support
    dest[copyLen] = '\0';
    return true;
}


bool ByteBuffer::toHex(char* dest,
                       size_t destCapacity) const {
    const size_t minReqCapacity = (this->length << 1)+1;

    if (!dest || destCapacity < minReqCapacity) return false;

    const char hexChars[] = "0123456789ABCDEF";

    size_t destIndex = 0;
    for (size_t i=0; i<this->length; i++) {
        // First 4-bits (High nibble)
        const size_t highNibbleValue = (this->bytes[i] >> 4) & 0x0F;

        // Last 4-bits (Low nibbl)
        const size_t lowNibbleValue = this->bytes[i] & 0x0F;

        dest[destIndex++] = hexChars[highNibbleValue];
        dest[destIndex++] = hexChars[lowNibbleValue];
    }

    dest[destIndex] = '\0';
    return true;
}


void ByteBuffer::dump() const {
    printf("\n--- ByteBuffer Dump (Length: %zu) ---\n", length);
    
    for (size_t i = 0; i < length; i += 16) {
        // 1. Print the memory offset (e.g., 0000, 0010)
        printf("%04zx: ", i);

        // 2. Print the Hex values
        for (size_t j = 0; j < 16; j++) {
            if (i + j < length)
                printf("%02X ", bytes[i + j]);
            else
                printf("   "); // Space if the buffer ends early
        }

        printf(" | ");

        // 3. Print the printable characters (Sanitized String)
        for (size_t j = 0; j < 16; j++) {
            if (i + j < length) {
                uint8_t b = bytes[i + j];
                printf("%c", (b >= 32 && b <= 126) ? (char)b : '.');
            }
        }
        printf("\n");
    }
    printf("--------------------------------------\n");
}

bool ByteBuffer::addByte(uint8_t byte) {
    if (!this->bytes || isFull()) return false;
    this->bytes[this->length++] = byte;
    return true;
}

bool ByteBuffer::setLength(size_t newLength) {
    if (newLength > this->capacity) return false;
    this->length = newLength;
    return true;
}

void ByteBuffer::setEndianOrder(Endian endianOrder) {
    this->order = endianOrder;
}

void ByteBuffer::clear() {
    this->length = 0;
}

void ByteBuffer::erase() {
    memset(this->bytes, 0, this->capacity);
    this->length=0;
}

bool ByteBuffer::isFull() const {
    return this->length >= this->capacity; 
}

size_t ByteBuffer::getSpaceLeft() const { 
    return this->capacity - this->length; 
}

const uint8_t* ByteBuffer::getRawBytes() const {
    return static_cast<const uint8_t*>(this->bytes);
}

uint8_t* ByteBuffer::getRawBytes() { 
    return this->bytes; 
}

size_t ByteBuffer::getSize() const {
    return this->length; 
}

size_t ByteBuffer::getCapacity() const { 
    return this->capacity;
} 

bool ByteBuffer::getByte(size_t index,
                         uint8_t& outByte) const {
    if (index >= this->length) return false;
    outByte = this->bytes[index];
    return true;
}

Endian ByteBuffer::getEndianOrder() const {
    return this->order; 
}

bool ByteBuffer::fromHex(const char* hexStr) {
    if (!hexStr) return false;
    size_t i = 0;

    size_t startLen = this->length;

    while (hexStr[i] != '\0') {
        // Skip common separators like space, colon, or dash
        if (hexStr[i] == ' ' || 
            hexStr[i] == ':' || 
            hexStr[i] == '-') {
            i++; continue;
        }

        // We need at least two characters for one byte
        if (hexStr[i + 1] == '\0') {
            this->length = startLen;
            return false;
        }

        uint8_t high = hexToNibble(hexStr[i]);
        uint8_t low = hexToNibble(hexStr[i + 1]);

        // If either character was not a valid hex digit (0-9, A-F)
        if (high > 15 || low > 15) {
            this->length = startLen;
            return false;
        }

        // Combine nibbles and add to buffer
        if (!addByte((high << 4) | low)) {
            this->length = startLen;
            return false;
        }

        i += 2;
    }
    return true;
}

uint8_t ByteBuffer::hexToNibble(char c) const {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 255; // Invalid
}


}