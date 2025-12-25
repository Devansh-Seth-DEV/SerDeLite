/*
 * SerDeLite
 * Copyright (C) 2025 Devansh Seth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 */

#ifndef SERDELITE_BYTEBUFFER_H
#define SERDELITE_BYTEBUFFER_H

#include "Common.h"
#include <stddef.h>
#include <stdint.h>

namespace serdelite {

/**
 * @name Core Memory Management
 * This is the "Engine Room" of your library, handling the raw memory
 * and data conversion.
 * @{
 */

/**
 * @class ByteBuffer
 * @brief A wrapper for raw memory buffers providing safe access and data conversion.
 * 
 * The ByteBuffer acts as the physical storage layer for SerDeLite. It manages 
 * a pointer to a raw `uint8_t` array, tracks the current data length, and 
 * provides utility functions to export data as Hexadecimal or ASCII strings.
 * 
 * @note This class does not manage memory allocation/deallocation; it simply 
 * 		 operates on memory provided by the user.
 */
class ByteBuffer {
public:
	/**
	 * @name Lifecycle & Initialization
	 * Methods responsible for setting up the memory and defining the behavior of the buffer.
	 * @{
	 */

	/**
	 * @brief Construct a new `ByteBuffer` object
	 * @param buffer The address of raw-memory used for storing bytes
	 * @param bufferCapacity The maximum capacity of memory
	 * @param endianOrder The endian order in which data should be written and
	 * 					  retrieved
	 * 
	 * @note The `ByteBuffer` object is not reponsible for the lifecycle of the
	 * 		 raw-memory buffer
	 */
	ByteBuffer(uint8_t* buffer,
               size_t bufferCapacity,
               Endian endianOrder = Endian::Big);

	/**
	 * @brief Set the Endian Order of `ByteBuffer` object
	 * 
	 * @param endianOrder The new endian order
	 * 
	 * @note This method should be called before any write operation, if called
	 * 		 after few write operations, then the data afterwards will get corrupted
	 * 		 It is recommended to use it safely and prefer to set the order at the
	 * 		 time of object-construction
	 */
	void setEndianOrder(Endian endianOrder);

	/** @} */

	
	/**
	 * @name Data Conversion & Debugging
	 * Functions that translate the raw binary data into human-readable formats or
	 * import data from strings.
	 * @{
	 */
	
	/**
	 * @brief Convert the raw bytes as a character string into the provided buffer
	 * 
	 * @param dest The destination buffer where the converted value
	 * 			   is going to be stored
	 * 
	 * @param destCapacity The maximum capacity of destination buffer
	 * 
	 * @return Returns `true` if the conversion is successfull, `false` if the 
	 * 		   `dest` is not valid or `destCapacity` is `0`
	 * 
	 * @note Give a valid `destCapacity` according to the data, as insufficient
	 * 		 capacity may lead to truncation of rest of the data
	 */
	bool toString(char* dest, size_t destCapacity) const;

	/**
	 * @brief Converts the raw bytes as a hexadecimal string into a provided buffer
	 * 
	 * @param dest The destination buffer where the converted value
	 * 			   is going to be stored

	 * @param destCapacity The maximum capacity of destination buffer
	 * 
	 * @return Returns `true` if the conversion is successfull, `false` if the 
	 * 		   `dest` is not valid or `destCapacity` is insufficient
	 * 
	 * @note The minimum required capacity for destination buffer should be
	 * 		 double the length of `ByteBuffer` plus 1 because each byte(8-bits)
	 * 		 is going to take 2-hex characters
	 * 		 For e.g to represent the bytes "1001 1010" the hexadecimal
	 * 		 representation for this will be "9A"
	 */
	bool toHex(char* dest, size_t destCapacity) const;

	/**
	 * @brief This function writes the bytes into buffer from the given hexadecimal string
	 * @param hexStr The hexadecimal string
	 * @return Returns `true` if the write operation is successful, `false` otherwise
	 */
	bool fromHex(const char* hexStr);

	/**
	 * @brief Outputs a formatted hexadecimal and ASCII representation of the buffer
	 * 		  to the console.
	 * 
	 * This function is a debugging tool that prints the raw memory content in a 16-byte
	 * wide table. Each row displays the memory offset, the hexadecimal values of the bytes,
	 * and a "sanitized" ASCII string where non-printable characters are replaced by dots (.).
	 * 
	 * @note This function prints directly to stdout (standard output).
	 */
	void dump() const;

	/** @} */

	/**
	 * @name Buffer State & Manipulation
	 * Methods that handle the physical writing of bytes and cursor management.
	 * @{
	 */

	/**
	 * @brief Allows `ByteBuffer` to new bytes of data
	 * @param byte The byte to be added
	 * @return Returns `true` if byte can be added, `false` if the buffer reached
	 * 		   it's maximum capacity
	 */
	bool addByte(uint8_t byte);

	/**
	 * @brief Manually update the length if raw bytes were modified externally
	 * @param newLength The new length to be updated
	 * @return Returns `true` if length is updated sucessfully, `false` if the 
	 * 		   given new length exceeds the `ByteBuffer` capacity
 	 */
	bool setLength(size_t newLength);


	/**
	 * @brief Resets the cursor of `ByteBuffer`
	 * 
	 * @warning This method does not erases the actual data from the buffer
	 * 			it just resets the cursor, to clear the data from the bytes
	 * 			use `erase()` method
	 */
	void clear();

	/**
	 * @brief Erase the data and resets the cursor of `ByteBuffer`
	 * @warning If you want to just resets the cursor use `clear()` method
	 */
	void erase();

	/** @} */

	/**
	 * @name Getters & Inspection
	 * Methods used to query the status and content of the buffer.
	 * @{
	 */

	/**
	 * @brief Check if the `ByteBuffer` is full
	 * @return Returns `true` if buffer is full, `false` otherwise
	 */
	bool isFull() const;

	/**
	 * @brief Getter method which gives how many more bytes can be added
	 * @return Returns the capacity that's left available to write more bytes
	 */ 
	size_t getSpaceLeft() const;

	/**
	 * @brief Getter method which gives read-only access to raw bytes of `ByteBuffer`
	 * @return Returns a const pointer to raw bytes for read-only
	 */
	const uint8_t* getRawBytes() const;

	/**
	 * @brief Getter method which gives read/write access to raw bytes of
	 * 		  `ByteBuffer`
	 * 
	 * @return Returns a pointer to raw bytes
	 * 
	 * @warning It is not recommended to use this for writing purpose as this can
	 * 			corrupt the actual data and the pointing if not used with
	 * 			proper case
	 * 			For writing purpose always use `addByte(uint8_t byte)` method
	 */
	uint8_t* getRawBytes();

	/**
	 * @brief Getter method which gives the size of information bytes
	 * @return Returns the size of data
	 */
	size_t getSize() const;

	/**
	 * @brief Getter method which gives the total capacity of current `ByteBuffer` object
	 * @return Returns the capacity of `ByteBuffer` object
	 */
	size_t getCapacity() const; 

	/**
	 * @brief Retrieves a byte at the specified index
	 * @param index The position to read from
	 * @param outByte Reference where the byte will be stored
	 * @return Returns `true` if index is valid, `false` if out of bounds
	 */
	bool getByte(size_t index, uint8_t& outByte) const;

	/**
	 * @brief Getter method which gives the Endian Order object of `ByteBuffer`
	 * @return Returns current Endian order of object
	 */
	Endian getEndianOrder() const;

	/** @} */


private:
	uint8_t *bytes;
	size_t length;
	size_t capacity;
	Endian order;

	/**
	 * @brief This function converts the a single hexadecimal character into a nibble(4-bit)
	 * @param c The hexadecimal character
	 * @return Returns corresponding nibble (uint8_t) of hex character
	 */
	uint8_t hexToNibble(char c) const;
};

/** @} */

} // namespace serdelite

#endif
