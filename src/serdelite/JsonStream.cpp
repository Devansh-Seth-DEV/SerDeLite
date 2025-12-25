#include "serdelite/JsonStream.h"
#include "serdelite/Common.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

namespace serdelite {

JsonStream::JsonStream(ByteBuffer& _buffer)
    : buffer(_buffer),
      isFirstField(true),
      isClosed(false)
{
    this->buffer
        .addByte(static_cast<uint8_t>('{'));
}

bool JsonStream::writeObject(const char* key,
                             const JsonSerializable& obj) {
    size_t startLen = this->buffer.getSize();
    if (!startField(key)) return false;

        
    if (!this->buffer
             .addByte(static_cast<uint8_t>('{'))) {
        this->buffer.setLength(startLen);
        return false;
    }

    // Save parent state
    bool parentFirst = this->isFirstField;
    bool parentClosed = this->isClosed;

    // Reset for the child (Child's first field needs no comma)
    this->isFirstField = true;
    this->isClosed = false;

    if (!obj.toJson(*this)) {
        this->buffer.setLength(startLen);
        this->isFirstField = parentFirst;
        this->isClosed = parentClosed;
        return false;
    }

    // Restore parent state
    this->isFirstField = parentFirst;
    this->isClosed = parentClosed;

    return true;
}

bool JsonStream::close() {
    if (this->isClosed) return true;

    if (!this->buffer
             .addByte(static_cast<uint8_t>('}')))
        return false;

    uint8_t* raw = this->buffer.getRawBytes();
    if (!raw) return false;

    if (this->buffer.getSize() < this->buffer.getCapacity()) {
        raw[this->buffer.getSize()] = '\0';
    }

    this->isClosed = true;
    return true;
}

bool JsonStream::writeUint8(const char* key, uint8_t val) {
    return writeIntBits(key,
                        static_cast<uint64_t>(val),
                        8,
                        false);
}

bool JsonStream::writeUint16(const char* key, uint16_t val) {
    return writeIntBits(key,
                        static_cast<uint64_t>(val),
                        16,
                        false);
}

bool JsonStream::writeUint32(const char* key, uint32_t val) {
    return writeIntBits(key,
                        static_cast<uint64_t>(val),
                        32,
                        false);
}

bool JsonStream::writeUint64(const char* key, uint64_t val) {
    return writeIntBits(key, val, 64, false);
}

bool JsonStream::writeInt8(const char* key, int8_t val) {
    return writeIntBits(key,
                        static_cast<uint64_t>(val),
                        8);
}

bool JsonStream::writeInt16(const char* key, int16_t val) {
    return writeIntBits(key,
                        static_cast<uint64_t>(val),
                        16);
}

bool JsonStream::writeInt32(const char* key, int32_t val) {
    return writeIntBits(key,
                        static_cast<uint64_t>(val),
                        32);
}

bool JsonStream::writeInt64(const char* key, int64_t val) {
    return writeIntBits(key,
                        static_cast<uint64_t>(val),
                        64);
}


bool JsonStream::writeFloat(const char* key, float val) {
    size_t startLen = this->buffer.getSize();
    if (!startField(key)) return false;

    if (!isfinite(val)) {
        if (!writeRaw("null", 4)) {
            this->buffer.setLength(startLen);
            return false;
        }
        return true;
    }

    char temp[32];
    int len = snprintf(temp, sizeof(temp), "%.9g", val);

    if (len < 0 || len >= (int)sizeof(temp) ||
        !writeRaw(temp, len)) {
        this->buffer.setLength(startLen);
        return false;
    }
    return true;
}

bool JsonStream::writeDouble(const char* key, double val) {
    size_t startLen = this->buffer.getSize();
    if (!startField(key)) return false;

    if (!isfinite(val)) {
        if (!writeRaw("null", 4)) {
            this->buffer.setLength(startLen);
            return false;
        }
        return true;
    }

    char temp[32];
    int len = snprintf(temp, sizeof(temp), "%.17g", val);
    
    if (len < 0 || len >= (int)sizeof(temp) ||
        !writeRaw(temp, len)) {
        this->buffer.setLength(startLen);
        return false;
    }
    return true;
}


bool JsonStream::writeBool(const char* key, bool val) {
    if (!startField(key)) return false;
    return val ? writeRaw("true", 4) : writeRaw("false", 5);
}

bool JsonStream::writeString(const char* key, const char* val) {
    if (!startField(key)) return false;
    
    if (!val) return writeRaw("null", 4);

    if (!this->buffer
             .addByte(static_cast<uint8_t>('\"'))) return false;

    if (!writeEscaped(val)) return false;
    
    return this->buffer
                .addByte(static_cast<uint8_t>('\"'));
}

JsonBuffer JsonStream::getJson() const {
    JsonBuffer jb(reinterpret_cast<const char*>(this->buffer.getRawBytes()),
                   this->buffer.getSize());

    return jb;
}

bool JsonStream::canWrite(size_t bytesCount) const {
    return this->buffer.getSpaceLeft() >= bytesCount;
}

bool JsonStream::writeIntBits(const char* key,
                              uint64_t val,
                              uint8_t bitSize,
                              bool isSigned) {
    if (bitSize == 0 || bitSize>64 || bitSize%8 != 0)
        return false;

    int startLen = this->buffer.getSize();

    if (!startField(key)) return false;

    char temp[24];
    int len=0;

    if (isSigned) {
        int64_t sVal;
        interpretAsSigned(val, bitSize, sVal);
        len = snprintf(temp, sizeof(temp), "%lld",
                       static_cast<long long>(sVal));
    } else {
        len = snprintf(temp, sizeof(temp), "%llu",
                       static_cast<unsigned long long>(val));
    }

    if (len < 0 || len >= (int)sizeof(temp) ||
        !writeRaw(temp, len)) {
        this->buffer.setLength(startLen);
        return false;
    }

    return true;
}

bool JsonStream::writeRaw(const char* str, size_t len) {
    if (!canWrite(len)) return false;

    for(size_t i=0; i<len; i++) {
        uint8_t byte = static_cast<uint8_t>(str[i]);
        this->buffer.addByte(byte);
    }
    return true;
}

bool JsonStream::writeEscaped(const char* str) {
    size_t startLen = this->buffer.getSize();
    bool success = true;

    while (*str) {
        bool isDefault = false;
    
        switch (*str) {
        case '\"': success = writeRaw("\\\"", 2); break;
        case '\\': success = writeRaw("\\\\", 2); break;
        case '\n': success = writeRaw("\\n", 2); break;
        case '\t': success = writeRaw("\\t", 2); break;
        case '\r': success = writeRaw("\\r", 2); break;
        case '\b': success = writeRaw("\\b", 2); break;
        case '\f': success = writeRaw("\\f", 2); break;
        default: isDefault = true; break;
        }

        if (isDefault && (unsigned char)*str < 0x20) {
            static const char hex[] = "0123456789ABCDEF";
            char esc[6] = {
                '\\', 'u', '0', '0',
                hex[((unsigned char)*str >> 4) & 0xF],
                hex[(unsigned char)*str & 0xF]
            };
            success = writeRaw(esc, 6);
        } else if (isDefault) {
            success = this->buffer.addByte(*str);
        }

        str++;
        if (!success) break;
    }

    if (!success) this->buffer.setLength(startLen);
    return success;
}


bool JsonStream::startField(const char* key) {
    if (this->isClosed || !key) return false;

    if (!this->isFirstField) {
        if (!this->buffer
                 .addByte(static_cast<uint8_t>(',')))
            return false;
    }

    this->isFirstField = false;

    if (!this->buffer
             .addByte(static_cast<uint8_t>('\"'))) return false;

    if (!writeRaw(key, strlen(key))) return false;
    return writeRaw("\":", 2);
}

}