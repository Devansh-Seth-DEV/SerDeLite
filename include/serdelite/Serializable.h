#ifndef SERDELITE_SERIALIZABLE_H
#define SERDELITE_SERIALIZABLE_H

#include <stddef.h>

namespace serdelite {

class ByteStream;
class JsonStream;

/**
 * @name Binary Serialization Interface
 * @brief Base class for objects that can be serialized to and from binary formats.
 * @{
 */

/**
 * @class ByteSerializable
 * @brief An abstract interface that enables binary serialization for custom classes.
 * 
 * Implementing this class allows an object to be used with ByteStream::writeObject 
 * and ByteStream::readObject.
 */
class ByteSerializable {
public:
    /** @brief Virtual destructor to ensure proper cleanup of derived classes. */
    virtual ~ByteSerializable() {}

    /**
     * @brief Serializes the object's data into a binary ByteStream.
     * @param stream The ByteStream to write data into.
     * @return true if all members were written successfully, false otherwise.
     * @note Implementation should write members in a consistent order.
     */
    virtual bool toByteStream(ByteStream& stream) const = 0;

    /**
     * @brief Deserializes data from a binary ByteStream into the object's members.
     * @param stream The ByteStream to read data from.
     * @return true if all members were read successfully, false otherwise.
     * @note Members must be read in the exact same order they were written.
     */
    virtual bool fromByteStream(ByteStream& stream) = 0;

    /**
     * @brief Calculates the total number of bytes required to store this object.
     * @return The size of the object in bytes.
     * @note This is used by the stream to verify if enough space exists before writing.
     */
    virtual size_t byteSize() const = 0;
};

/** @} */



/**
 * @name JSON Serialization Interface
 * @brief Base class for objects that can be represented as JSON.
 * @{
 */

/**
 * @class JsonSerializable
 * @brief An abstract interface that enables JSON serialization for custom classes.
 * 
 * Implementing this class allows an object to be used with JsonStream::writeObject.
 */
class JsonSerializable {
public:
    /** @brief Default virtual destructor. */
    virtual ~JsonSerializable() = default;

    /**
     * @brief Public entry point to trigger JSON serialization.
     * @param stream The JsonStream to write the JSON text into.
     * @return true if the serialization process was successful.
     * @note This method typically wraps the call to serializeToJson with 
     *       necessary JSON structural elements.
     */
    bool toJson(JsonStream& stream) const;

protected:
    /**
     * @brief Pure virtual method that defines the specific JSON fields for the object.
     * @param stream The JsonStream where key-value pairs should be written.
     * @return true if the members were serialized correctly.
     * @note This must be implemented by the derived class using JsonStream's write methods.
     */
    virtual bool serializeToJson(JsonStream& stream) const = 0;
};

/** @} */
    
} // namespace serdelite

#endif
