#ifndef SERDELITE_JSONSTREAM_H
#define SERDELITE_JSONSTREAM_H

#include "ByteBuffer.h"
#include "JsonBuffer.h"
#include "Serializable.h"
#include <stddef.h>

namespace serdelite {

/**
 * @name JSON Streaming
 * This is the "Textual Serializer," turning objects into formatted JSON strings.
 * @{
 */

/**
 * @class JsonStream
 * @brief A stream-oriented interface for constructing JSON-formatted strings.
 * 
 * JsonStream allows for the creation of JSON objects directly into a ByteBuffer. 
 * Unlike the binary stream, this class is key-value oriented, ensuring that 
 * data is stored in a human-readable format compatible with web services 
 * and configuration files.
 * 
 * @sa JsonBuffer
 * @sa ByteBuffer
 * @sa JsonSerializable
 */
class JsonStream {
public:
	/**
	 * @name Lifecycle & Finalization
	 * Functions for initializing the JSON stream and finalizing the string format.
	 * @{
	 */

	/**
	 * @brief Construct a new `JsonStream` object.
	 * @param _buffer The ByteBuffer where the JSON string will be constructed.
	 * @note This constructor automatically writes the opening brace '{' to the buffer.
	 */
	JsonStream(ByteBuffer& _buffer);

	/**
	 * @brief Finalizes the JSON object by adding the closing brace.
	 * @return true if the closing brace was successfully written, false otherwise.
	 * @note Once closed, no more data should be written to this stream.
	 */
	bool close();

	/**
	 * @brief Retrieves the constructed JSON as a JsonBuffer object.
	 * @return A JsonBuffer containing a pointer to the string and its total length.
	 * @note Ensure close() has been called before using this to get a valid JSON object.
	 */
	JsonBuffer getJson() const;

	/** @} */


	/**
	 * @name JSON Primitives
	 * Functions for writing standard data types as JSON key-value pairs.
	 * @{
	 */

	/**
	 * @brief Writes an unsigned 8-bit integer.
	 * @param key The JSON field name.
	 * @param val The value to write.
	 * @return true if written successfully.
	 */
	bool writeUint8(const char* key, uint8_t val);

	/**
	 * @brief Writes an unsigned 16-bit integer as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The uint16_t value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 */
	bool writeUint16(const char* key, uint16_t val);

	/**
	 * @brief Writes an unsigned 32-bit integer as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The uint32_t value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 */
	bool writeUint32(const char* key, uint32_t val);

	/**
	 * @brief Writes an unsigned 64-bit integer as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The uint64_t value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 */
	bool writeUint64(const char* key, uint64_t val);

	/**
	 * @brief Writes a signed 8-bit integer as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The int8_t value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 */
	bool writeInt8(const char* key, int8_t val);

	/**
	 * @brief Writes a signed 16-bit integer as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The int16_t value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 */
	bool writeInt16(const char* key, int16_t val);

	/**
	 * @brief Writes a signed 32-bit integer as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The int32_t value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 */
	bool writeInt32(const char* key, int32_t val);

	/**
	 * @brief Writes a signed 64-bit integer as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The int64_t value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 */
	bool writeInt64(const char* key, int64_t val);

	/**
	 * @brief Writes a 32-bit floating point number as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The float value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 * @note Precision is handled based on the internal implementation of
	 * 		 the JSON string converter.
	 */
	bool writeFloat(const char* key, float val);

	/**
	 * @brief Writes a 64-bit floating point number (double) as a JSON key-value pair.
	 * @param key The JSON field name (string).
	 * @param val The double value to convert to text.
	 * @return true if successfully written to the buffer, false if capacity exceeded.
	 */
	bool writeDouble(const char* key, double val);

	/**
	 * @brief Writes a boolean value as 'true' or 'false' text.
	 * @param key The JSON field name.
	 * @param val The bool value.
	 * @return true if successful.
	 */
	bool writeBool(const char* key, bool val);

	/**
	 * @brief Writes a string value enclosed in double quotes.
	 * @param key The JSON field name.
	 * @param val The string value (null-terminated).
	 * @return true if successful.
	 */
	bool writeString(const char* key, const char* val);

	/** @} */


	/**
	 * @name Object Serialization
	 * Functions for nesting complex objects within the JSON structure.
	 * @{
	 */

	/**
	 * @brief Serializes a custom object as a nested JSON object.
	 * 
	 * This calls the object's `toJsonStream` method and wraps it in braces 
	 * under the provided key.
	 * 
	 * @param key The JSON field name for the nested object.
	 * @param obj The JsonSerializable object to serialize.
	 * @return true if the object was successfully nested.
	 */
	bool writeObject(const char* key, const JsonSerializable& obj);

	/** @} */


	/**
	 * @name Stream Safety
	 * Functions to monitor buffer capacity during string construction.
	 * @{
	 */

	/**
	 * @brief Checks if there is enough space in the ByteBuffer for the text.
	 * @param bytesCount Estimated number of characters to write.
	 * @return true if the buffer has enough capacity left.
	 */
	bool canWrite(size_t bytesCount) const;

	/** @} */

private:
	ByteBuffer& buffer;
	bool isFirstField;
	bool isClosed;

	bool writeIntBits(const char* key, uint64_t val,
					  uint8_t bitSize, bool isSigned = true);

	bool writeRaw(const char* str, size_t len);

	bool writeEscaped(const char* str);

	bool startField(const char* key);
};

/** @} */

} // namespace serdelite

#endif