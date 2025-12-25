#include "serdelite/ByteStream.h"
#include "serdelite/Common.h"

#include <string.h>

namespace serdelite {

ByteStream::ByteStream(ByteBuffer& _buffer)
    : buffer(_buffer), readPos(0)
{

}

bool ByteStream::writeLibraryHeader() {
    /*
        Write: 
            Magic Number (4 bytes) + 
            Major (1) + 
            Minor (1) + 
            Patch (1) 
            -------------------------
            = 7 bytes total
    */
    if (!canWrite(7)) return false; 

    writeUint32(SERDELITE_MAGIC);
    writeUint8(SERDELITE_VERSION_MAJOR);
    writeUint8(SERDELITE_VERSION_MINOR);
    writeUint8(SERDELITE_VERSION_PATCH);
    
    return true;
}

bool ByteStream::verifyLibraryHeader() {
    // Total header size is 7 bytes
    if (!canRead(7)) return false;

    // Save current read position in case we need to roll back
    size_t startPos = this->readPos;

    uint32_t magic;
    uint8_t major, minor, patch;

    // Check Magic Number
    if (!readUint32(magic) || magic != SERDELITE_MAGIC) {
        this->readPos = startPos;
        return false;
    }

    // Read Version Components
    if (!readUint8(major) || !readUint8(minor) || !readUint8(patch)) {
        this->readPos = startPos;
        return false;
    }

    // Version Compatibility Check
    // Usually, we only care if the Major version matches.
    if (major != SERDELITE_VERSION_MAJOR) {
        this->readPos = startPos;
        return false;
    }

    return true;
}

bool ByteStream::peekUint32(uint32_t& out) const {
    if (!canRead(4)) return false;

    uint64_t value = 0;

    // We use a local offset instead of modifying this->readPos
    size_t peekPos = this->readPos;

    if (this->buffer.getEndianOrder() == Endian::Big) {
        for (int16_t i = 24; i >= 0; i -= 8) {
            uint8_t byte;
            this->buffer.getByte(peekPos++, byte);
            value |= (static_cast<uint64_t>(byte) << i);
        }
    } else {
        for (int16_t i = 0; i <= 24; i += 8) {
            uint8_t byte;
            this->buffer.getByte(peekPos++, byte);
            value |= (static_cast<uint64_t>(byte) << i);
        }
    }

    out = static_cast<uint32_t>(value);
    return true;
}

bool ByteStream::isSerdeliteBuffer() const {
    uint32_t magic = 0;
    if (peekUint32(magic)) {
        return magic == SERDELITE_MAGIC;
    }
    return false;
}

bool ByteStream::writeObject(const ByteSerializable& obj) {
    return obj.toByteStream(*this);
}

bool ByteStream::readObject(ByteSerializable& obj) {
    return obj.fromByteStream(*this);
}

bool ByteStream::writeUint8(uint8_t val) {
    return this->buffer.addByte(val);
}

bool ByteStream::writeUint16(uint16_t val) {
    return writeBits(static_cast<uint16_t>(val),
                     16);
}

bool ByteStream::writeUint32(uint32_t val) {
    return writeBits(static_cast<uint32_t>(val),
                     32);
}

bool ByteStream::writeUint64(uint64_t val) {
    return writeBits(val);
}

bool ByteStream::writeInt8(int8_t val) {
    return writeBits(static_cast<uint8_t>(val),
                     8);
}

bool ByteStream::writeInt16(int16_t val) {
    return writeBits(static_cast<uint16_t>(val),
                     16);
}

bool ByteStream::writeInt32(int32_t val) {
    return writeBits(static_cast<uint32_t>(val),
                     32);
}

bool ByteStream::writeInt64(int64_t val) {
    return writeBits(static_cast<uint64_t>(val));
}

bool ByteStream::writeFloat(float val) {
    uint32_t bits;
    memcpy(&bits, &val, sizeof(float));
    return writeBits(bits, 32);
}

bool ByteStream::writeDouble(double val) {
    uint64_t bits;
    memcpy(&bits, &val, sizeof(double));
    return writeUint64(bits);
}

bool ByteStream::writeChars(const char* str, size_t length) {
    if (!str) return false;
    if (!canWrite(length)) return false;

    for(size_t i=0; i<length; i++) {
        uint8_t byte = static_cast<uint8_t>(str[i]);
        this->buffer.addByte(byte);
    }
    return true;
}

bool ByteStream::writeString(const char* str) {
    if (!str) return writeUint16(0);

    size_t len = strlen(str);
    
    constexpr size_t UINT16_MAX_VALUE = 0xFFFF;
    if (len > UINT16_MAX_VALUE ||
        !canWrite(sizeof(uint16_t) + len)) return false;
    
    size_t startLen = this->buffer.getSize();
    if (!writeUint16(static_cast<uint16_t>(len)) ||
        !writeChars(str, len)) {
        this->buffer.setLength(startLen);
        return false;
    }
    return true;
}

bool ByteStream::writeBool(bool val) {
    return writeUint8(val ? 1 : 0);
}

bool ByteStream::readUint8(uint8_t& out) {
    if (!canRead(sizeof(uint8_t))) return false;

    if (this->buffer
            .getByte(this->readPos, out)) {
        this->readPos++;
        return true;
    }
    return false;
}

bool ByteStream::readUint16(uint16_t& out) {
    if (!canRead(sizeof(uint16_t))) return false;

    uint64_t val;
    if (!readBits(val, 16)) return false;
    out = static_cast<uint16_t>(val);
    return true;
}

bool ByteStream::readUint32(uint32_t& out) {
    if (!canRead(sizeof(uint32_t))) return false;

    uint64_t val;
    if (!readBits(val, 32)) return false;
    out = static_cast<uint32_t>(val);
    return true;
}

bool ByteStream::readUint64(uint64_t& out) {
    if (!canRead(sizeof(uint64_t))) return false;
    if (!readBits(out)) return false;
    return true;
}

bool ByteStream::readInt8(int8_t& out) {
    if (!canRead(sizeof(int8_t))) return false;

    uint8_t val;
    if (!readUint8(val)) return false;

    int64_t sVal;
    interpretAsSigned(static_cast<uint64_t>(val),
                      8,
                      sVal);

    out = static_cast<int8_t>(val);
    return true;
}

bool ByteStream::readInt16(int16_t& out) {
    if (!canRead(sizeof(int16_t))) return false;

    uint16_t val;
    if (!readUint16(val)) return false;
    
    int64_t sVal;
    interpretAsSigned(static_cast<uint64_t>(val),
                      16,
                      sVal); 

    out = static_cast<int16_t>(sVal);
    return true;
}

bool ByteStream::readInt32(int32_t& out) {
    if (!canRead(sizeof(int32_t))) return false;

    uint32_t val;
    if (!readUint32(val)) return false;

    int64_t sVal;
    interpretAsSigned(static_cast<uint64_t>(val),
                      32,
                      sVal);

    out = static_cast<int32_t>(sVal);
    return true;
}

bool ByteStream::readInt64(int64_t& out) {
    if (!canRead(sizeof(int64_t))) return false;

    uint64_t val;
    if (!readUint64(val)) return false;
    out = static_cast<int64_t>(val);
    return true;
}

bool ByteStream::readFloat(float& out) {
    if (!canRead(sizeof(float))) return false;

    uint32_t bits;
    if (!readUint32(bits)) return false;
    memcpy(&out, &bits, sizeof(float));
    return true;
}

bool ByteStream::readDouble(double& out) {
    if (!canRead(sizeof(double))) return false;

    uint64_t bits;
    if (!readUint64(bits)) return false;
    memcpy(&out, &bits, sizeof(double));
    return true;
}

bool ByteStream::readChars(char* dest, size_t length) {
    if (!canRead(length)) return false;

    for(size_t i=0; i<length; i++) {
        uint8_t byte;
        readUint8(byte);
        dest[i] = static_cast<char>(byte);
    }
    return true;
}

bool ByteStream::readString(char* dest, size_t destCapacity) {
    uint16_t len;
    if (!readUint16(len)) return false;

    if (destCapacity < (size_t)len + 1)
        return false;

    if (!readChars(dest, len)) return false;

    dest[len] = '\0';
    return true;
}

bool ByteStream::readBool(bool& out) {
    uint8_t val;
    if (!readUint8(val)) return false;
    out = (val != 0) ? true : false;
    return true;
}

void ByteStream::resetReadCursor() {
    this->readPos = 0;
}

bool ByteStream::canRead(size_t bytesCount) const {
    return (this->readPos + bytesCount) <=
            this->buffer.getSize();
}

bool ByteStream::canWrite(size_t bytesCount) const {
    return this->buffer.getSpaceLeft() >= bytesCount;
}

bool ByteStream::writeBits(uint64_t val, uint8_t bitSize) {
    if (bitSize == 0 || (bitSize % 8) != 0) return false;
    if (!canWrite(bitSize/8)) return false;

    if (buffer.getEndianOrder() == Endian::Big) {
        int16_t start = static_cast<int16_t>(bitSize)-8;
        
        for(int16_t i=start; i>=0; i-=8) {
            uint8_t byte = static_cast<uint8_t>(
                                (val >> i) & 0xFF
                            );
            this->buffer.addByte(byte);
        }
    } else {
        int16_t end = static_cast<int16_t>(bitSize);

        for(int16_t i=0; i<end; i+=8) {
            uint8_t byte = static_cast<uint8_t>(
                                (val >> i) & 0xFF
                            );
            this->buffer.addByte(byte);
        }
    }
    return true;
}

bool ByteStream::readBits(uint64_t& out, uint8_t bitSize) {
    if (bitSize == 0 || (bitSize % 8) != 0) return false;
    if (!canRead(bitSize/8)) return false;

    uint64_t value = 0;
    size_t initReadPos = this->readPos;

    if (this->buffer.getEndianOrder() == Endian::Big) {
        int16_t start = static_cast<int16_t>(bitSize)-8;

        for(int16_t i=start; i>=0; i-=8) {
            uint8_t byte;
            if(!this->buffer
                    .getByte(this->readPos++, byte)) {
                this->readPos = initReadPos;
                return false;
            }

            value |= (static_cast<uint64_t>(byte) << i);
        }
    } else {
        int16_t end = static_cast<int16_t>(bitSize);

        for (int16_t i=0; i<end; i+=8) {
            uint8_t byte;
            if (!this->buffer
                     .getByte(this->readPos++, byte)) {
                this->readPos = initReadPos;
                return false;
            }
            
            value |= (static_cast<uint64_t>(byte) << i);
        }			
    }

    out = value;
    return true;
}


}