#ifndef SERDELITE_BYTESTREAM_H
#define SERDELITE_BYTESTREAM_H

#include "Version.h"
#include "ByteBuffer.h"
#include "Serializable.h"

#include <stddef.h>

namespace serdelite {

/**
 * @name Binary Streaming
 * This is the "Binary Translator," providing the logic for reading and writing data
 * in a specific endianness.
 * @{
 */

/**
 * @class ByteStream
 * @brief A stream-oriented interface for reading and writing binary data.
 * 
 * ByteStream provides a high-level API to manipulate a ByteBuffer. It handles 
 * the internal read/write cursors and performs automatic endianness conversion 
 * for all primitive types. It is designed for high-performance, compact 
 * data serialization.
 */
class ByteStream {
public:
	ByteStream(ByteBuffer& _buffer);

	/**
	 * @name Stream Metadata & Validation
	 * Functions used to identify, version-stamp, and verify the integrity 
	 * of SerDeLite binary streams.
	 * @{
	 */

	/**
	 * @brief Writes the SerDeLite Metadata into the stream, It is recommended to
	 * 		  use it if before writing any data into stream as it get's useful while
	 * 		  sending the data over network as it'll help the reciever side to
	 * 		  deserialize the data using the corresponding SerDeLite library version
	 * 
	 * @return Returns `true` if metadata added to stream, `false` otherwise
	 */
	bool writeLibraryHeader();

	/**
	 * @brief Checkes for the SerDeLite Metadata at the starting of the stream
	 * 		  It is recommended to use it before start reading data from stream
	 * 		  if you are now sure whether the current SerDeLite version is compatible 
	 * 		  with the binary data packed into the `ByteStream` object
	 * 
	 * @return Returns `true` if the metadata is matched with the current
	 * 		   SerDeLite version, `false` otherwise 
	 */
	bool verifyLibraryHeader();

	/**
	 * @brief Checks if the buffer starts with the SerDelite Magic Number.
	 * @return Returns `true` if the magic matches, `false` otherwise.
	 */
	bool isSerdeliteBuffer() const;

	/** @} */


	/**
	 * @name Stream Inspection
	 * 
	 * Functions that allow looking at data without consuming it or 
	 * moving the internal cursors.
	 * 
	 * @{
	 */

	/**
	 * @brief Reads a 32-bit value without advancing the read cursor.
	 * @param out Reference to store the value.
	 * @return Returns `true` if there were enough bytes to peek, `false` otherwise.
	 */
	bool peekUint32(uint32_t& out) const;

	/** @} */


	/**
	 * @name Object Serialization
	 * 
	 * Functions designed to handle custom complex structures that implement 
	 * the ByteSerializable interface.
	 * 
	 * @{
	 */

	/**
	 * @brief Serializes a custom object into the stream.
	 * 
	 * This method acts as a bridge; it calls the object's internal `toByteStream` 
	 * implementation. This allows for clean, nested serialization of complex classes.
	 * 
	 * @param obj A reference to the ByteSerializable object to be written.
	 * @return Returns `true` if the object was successfully written, `false` otherwise.
	 */
	bool writeObject(const ByteSerializable& obj);

	/**
	 * @brief Deserializes data from the stream into a custom object.
	 * 
	 * This method calls the object's `fromByteStream` implementation. It will 
	 * fill the object's members with data read sequentially from the current 
	 * position of the read cursor.
	 * 
	 * @param obj A reference to the ByteSerializable object to be populated.
	 * @return Returns `true` if the object was successfully read, `false` otherwise.
	 */
	bool readObject(ByteSerializable& obj);

	/** @} */


	/**
	 * @name Write Primitives
	 * 
	 * Low-level functions for writing fixed-width fundamental types into the stream.
	 * 
	 * @{
	 */

	/**
	 * @brief Writes an unsigned 8-bit integer.
	 * @param val The uint8_t value to write.
	 * @return true if successful, false if buffer is full.
	 */
	bool writeUint8(uint8_t val);

	/**
	 * @brief Writes an unsigned 16-bit integer.
	 * @param val The uint16_t value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Uses buffer's(passed to constructor) endian order for conversion.
	 */
	bool writeUint16(uint16_t val);

	/**
	 * @brief Writes an unsigned 32-bit integer.
	 * @param val The uint32_t value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Uses buffer's(passed to constructor) endian order for conversion.
	 */
	bool writeUint32(uint32_t val);

	/**
	 * @brief Writes an unsigned 64-bit integer.
	 * @param val The uint64_t value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Uses buffer's(passed to constructor) endian order for conversion.
	 */
	bool writeUint64(uint64_t val);

	/**
	 * @brief Writes a signed 8-bit integer.
	 * @param val The int8_t value to write.
	 * @return true if successful, false if buffer is full.
	 */
	bool writeInt8(int8_t val);

	/**
	 * @brief Writes a signed 16-bit integer.
	 * @param val The int16_t value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Uses buffer's(passed to constructor) endian order for conversion.
	 */
	bool writeInt16(int16_t val);

	/**
	 * @brief Writes a signed 32-bit integer.
	 * @param val The int32_t value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Uses buffer's(passed to constructor) endian order for conversion.
	 */
	bool writeInt32(int32_t val);

	/**
	 * @brief Writes a signed 64-bit integer.
	 * @param val The int64_t value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Uses buffer's(passed to constructor) endian order for conversion.
	 */
	bool writeInt64(int64_t val);

	/**
	 * @brief Writes a 32-bit floating point number.
	 * @param val The float value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Uses buffer's(passed to constructor) endian order for conversion.
	 */
	bool writeFloat(float val);

	/**
	 * @brief Writes a 64-bit floating point number.
	 * @param val The double value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Uses buffer's(passed to constructor) endian order for conversion.
	 */
	bool writeDouble(double val);

	/**
	 * @brief Writes a raw sequence of characters without a length prefix.
	 * @param str Pointer to the character array.
	 * @param length Number of characters to write.
	 * @return true if successful, false if buffer overflow occurs.
	 * @note This does NOT write a null-terminator or length prefix.
	 */
	bool writeChars(const char* str, size_t length);

	/**
	 * @brief Writes a string with an automatic 16-bit length prefix.
	 * @param str The null-terminated string to write.
	 * @return true if successful, false if buffer overflow occurs.
	 * @note The length is stored as a uint16_t before the string data. 
	 * 		 Does not write the null-terminator character.
	 */
	bool writeString(const char* str);

	/**
	 * @brief Writes a boolean value as a single byte.
	 * @param val The bool value to write.
	 * @return true if successful, false if buffer is full.
	 * @note Writes 0x01 for true and 0x00 for false.
	 */
	bool writeBool(bool val);

	/** @} */

	
	/**
	 * @name Read Primitives
	 * Functions for extracting fixed-width fundamental types from the stream.
	 * @{
	 */

	/**
	 * @brief Reads an unsigned 8-bit integer from the stream.
	 * @param[out] out Reference to store the retrieved uint8_t value.
	 * @return true if successful, false if there are not enough bytes left.
	 */
	bool readUint8(uint8_t& out);

	/**
	 * @brief Reads an unsigned 16-bit integer and handles endianness.
	 * @param[out] out Reference to store the retrieved uint16_t value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Automatically converts from stream buffer's Endian to Host byte order.
	 */
	bool readUint16(uint16_t& out);

	/**
	 * @brief Reads an unsigned 32-bit integer and handles endianness.
	 * @param[out] out Reference to store the retrieved uint32_t value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Automatically converts from stream buffer's Endian to Host byte order.
	 */
	bool readUint32(uint32_t& out);

	/**
	 * @brief Reads an unsigned 64-bit integer and handles endianness.
	 * @param[out] out Reference to store the retrieved uint64_t value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Automatically converts from stream buffer's Endian to Host byte order.
	 */
	bool readUint64(uint64_t& out);

	/**
	 * @brief Reads a signed 8-bit integer from the stream.
	 * @param[out] out Reference to store the retrieved int8_t value.
	 * @return true if successful, false if there are not enough bytes left.
	 */
	bool readInt8(int8_t& out);

	/**
	 * @brief Reads a signed 16-bit integer and handles endianness.
	 * @param[out] out Reference to store the retrieved int16_t value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Automatically converts from stream buffer's Endian to Host byte order.
	 */
	bool readInt16(int16_t& out);

	/**
	 * @brief Reads a signed 32-bit integer and handles endianness.
	 * @param[out] out Reference to store the retrieved int32_t value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Automatically converts from stream buffer's Endian to Host byte order.
	 */
	bool readInt32(int32_t& out);

	/**
	 * @brief Reads a signed 64-bit integer and handles endianness.
	 * @param[out] out Reference to store the retrieved int64_t value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Automatically converts from stream buffer's Endian to Host byte order.
	 */
	bool readInt64(int64_t& out);

	/**
	 * @brief Reads a 32-bit floating point number.
	 * @param[out] out Reference to store the retrieved float value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Interprets the bits in according to the stream buffer's endian format.
	 */
	bool readFloat(float& out);

	/**
	 * @brief Reads a 64-bit floating point number.
	 * @param[out] out Reference to store the retrieved double value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Interprets the bits in according to the stream buffer's endian format.
	 */
	bool readDouble(double& out);

	/**
	 * @brief Reads a raw sequence of characters into a destination buffer.
	 * @param[out] dest Pointer to the memory where characters will be copied.
	 * @param length The exact number of characters to read.
	 * @return true if successful, false if there are not enough bytes in the stream.
	 * @note This does NOT append a null-terminator. Ensure 'dest' is large enough.
	 */
	bool readChars(char* dest, size_t length);

	/**
	 * @brief Reads a length-prefixed string and ensures null-termination.
	 * @param[out] dest Pointer to the character array to store the string.
	 * @param destCapacity The maximum size of the 'dest' buffer.
	 * @return true if successful, false if stream is truncated or 'dest' is too small.
	 * @note This function reads the uint16_t length prefix first. It automatically
	 * 		 adds a '\0' at the end of the string in 'dest'.
	 */
	bool readString(char* dest, size_t destCapacity);

	/**
	 * @brief Reads a boolean value stored as a single byte.
	 * @param[out] out Reference to store the retrieved bool value.
	 * @return true if successful, false if there are not enough bytes left.
	 * @note Any non-zero byte is interpreted as true; 0x00 is false.
	 */
	bool readBool(bool& out);

	/** @} */


	/**
	 * @name Stream Management & Safety
	 * Functions used to manage the internal state of the stream and verify 
	 * available space before performing operations.
	 * @{
	 */

	/**
	 * @brief Resets the internal read cursor back to the beginning of the stream.
	 * 
	 * Use this when you need to re-read the data from the start of the buffer
	 * without re-writing the content. This only affects the read operations;
	 * the write cursor remains at its current position.
	 */
	void resetReadCursor();

	/**
	 * @brief Checks if there are enough bytes left in the buffer to perform a read.
	 * @param bytesCount The number of bytes you intend to read.
	 * @return true if the requested number of bytes is available between the 
	 * 		   current read cursor and the end of the written data.
	 * @return false if the read would exceed the buffer limits (Buffer Underflow).
	 */
	bool canRead(size_t bytesCount) const;

	/**
	 * @brief Checks if there is enough capacity left in the buffer to perform a write.
	 * @param bytesCount The number of bytes you intend to write.
	 * @return true if the buffer has enough physical space remaining from the 
	 * 		   current write cursor to the end of the allocated memory.
	 * @return false if the write would exceed the buffer's capacity (Buffer Overflow).
	 */
	bool canWrite(size_t bytesCount) const;

	/** @} */
	
private:
	ByteBuffer& buffer;
	size_t readPos;

	bool writeBits(uint64_t val, uint8_t bitSize = 64);

	bool readBits(uint64_t& out, uint8_t bitSize = 64);
};

/** @} */

} // namespace serdelite

#endif