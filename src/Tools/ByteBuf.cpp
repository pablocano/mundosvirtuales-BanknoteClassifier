#include "ByteBuf.h"

/**
 * ByteBuf constructor
 * Reserves specified size in internal vector
 *
 * @param size Size (in bytes) of space to preallocate internally. Default is set in DEFAULT_SIZE
 */
ByteBuf::ByteBuf(uint32_t size) {
	buf.reserve(size);
	clear();
}

/**
 * ByteBuf constructor
 * Consume an entire uint8_t array of length len in the ByteBuf
 *
 * @param arr uint8_t array of data (should be of length len)
 * @param size Size of space to allocate
 */
ByteBuf::ByteBuf(uint8_t* arr, uint32_t size) {
	// If the provided array is NULL, allocate a blank buffer of the provided size
	if (arr == NULL) {
		buf.reserve(size);
		clear();
	} else { // Consume the provided array
		buf.reserve(size);
		clear();
		putBytes(arr, size);
	}
}

/**
 * Bytes Remaining
 * Returns the number of bytes from the current read position till the end of the buffer
 *
 * @return Number of bytes from rpos to the end (size())
 */
uint32_t ByteBuf::bytesRemaining() {
	return size() - rpos;
}

/**
 * Clear
 * Clears out all data from the internal vector (original preallocated size remains), resets the positions to 0
 */
void ByteBuf::clear() {
	rpos = 0;
	wpos = 0;
	buf.clear();
}

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * Clone
 * Allocate an exact copy of the ByteBuf on the heap and return a pointer
 *
 * @return A pointer to the newly cloned ByteBuf. NULL if no more memory available
 */
std::unique_ptr<ByteBuf> ByteBuf::clone() {
    std::unique_ptr<ByteBuf> ret = make_unique<ByteBuf>((uint32_t) buf.size());

	// Copy data
	for (uint32_t i = 0; i < buf.size(); i++) {
		ret->put((uint8_t) get(i));
	}

	// Reset positions
	ret->setReadPos(0);
	ret->setWritePos(0);

	return ret;
}

/**
 * Equals, test for data equivilancy
 * Compare this ByteBuf to another by looking at each byte in the internal buffers and making sure they are the same
 *
 * @param other A pointer to a ByteBuf to compare to this one
 * @return True if the internal buffers match. False if otherwise
 */
bool ByteBuf::equals(ByteBuf* other) {
	// If sizes aren't equal, they can't be equal
	if (size() != other->size())
		return false;

	// Compare byte by byte
	uint32_t len = size();
	for (uint32_t i = 0; i < len; i++) {
		if ((uint8_t) get(i) != (uint8_t) other->get(i))
			return false;
	}

	return true;
}

/**
 * Resize
 * Reallocates memory for the internal buffer of size newSize. Read and write positions will also be reset
 *
 * @param newSize The amount of memory to allocate
 */
void ByteBuf::resize(uint32_t newSize) {
	buf.resize(newSize);
	rpos = 0;
	wpos = 0;
}

/**
 * Size
 * Returns the size of the internal buffer...not necessarily the length of bytes used as data!
 *
 * @return size of the internal buffer
 */
uint32_t ByteBuf::size() {
	return (uint32_t) buf.size();
}

// Replacement

/**
 * Replace
 * Replace occurance of a particular uint8_t, key, with the uint8_t rep
 *
 * @param key uint8_t to find for replacement
 * @param rep uint8_t to replace the found key with
 * @param start Index to start from. By default, start is 0
 * @param firstOccuranceOnly If true, only replace the first occurance of the key. If false, replace all occurances. False by default
 */
void ByteBuf::replace(uint8_t key, uint8_t rep, uint32_t start, bool firstOccuranceOnly) {
	uint32_t len = (uint32_t) buf.size();
	for (uint32_t i = start; i < len; i++) {
		uint8_t data = read<uint8_t>(i);
		// Wasn't actually found, bounds of buffer were exceeded
		if ((key != 0) && (data == 0))
			break;

		// Key was found in array, perform replacement
		if (data == key) {
			buf[i] = rep;
			if (firstOccuranceOnly)
				return;
		}
	}
}

// Read Functions

uint8_t ByteBuf::peek() const {
	return read<uint8_t>(rpos);
}

uint8_t ByteBuf::get() const {
	return read<uint8_t>();
}

uint8_t ByteBuf::get(uint32_t index) const {
	return read<uint8_t>(index);
}

void ByteBuf::getBytes(uint8_t* buf, uint32_t len) const {
	for (uint32_t i = 0; i < len; i++) {
		buf[i] = read<uint8_t>();
	}
}

char ByteBuf::getChar() const {
	return read<char>();
}

char ByteBuf::getChar(uint32_t index) const {
	return read<char>(index);
}

double ByteBuf::getDouble() const {
	return read<double>();
}

double ByteBuf::getDouble(uint32_t index) const {
	return read<double>(index);
}

float ByteBuf::getFloat() const {
	return read<float>();
}

float ByteBuf::getFloat(uint32_t index) const {
	return read<float>(index);
}

uint32_t ByteBuf::getInt() const {
	return read<uint32_t>();
}

uint32_t ByteBuf::getInt(uint32_t index) const {
	return read<uint32_t>(index);
}

uint64_t ByteBuf::getLong() const {
	return read<uint64_t>();
}

uint64_t ByteBuf::getLong(uint32_t index) const {
	return read<uint64_t>(index);
}

uint16_t ByteBuf::getShort() const {
	return read<uint16_t>();
}

uint16_t ByteBuf::getShort(uint32_t index) const {
	return read<uint16_t>(index);
}

// Write Functions

void ByteBuf::put(ByteBuf* src) {
	uint32_t len = src->size();
	for (uint32_t i = 0; i < len; i++)
		append<uint8_t>(src->get(i));
}

void ByteBuf::put(uint8_t b) {
	append<uint8_t>(b);
}

void ByteBuf::put(uint8_t b, uint32_t index) {
	insert<uint8_t>(b, index);
}

void ByteBuf::putBytes(uint8_t* b, uint32_t len) {
	// Insert the data one byte at a time into the internal buffer at position i+starting index
	for (uint32_t i = 0; i < len; i++)
		append<uint8_t>(b[i]);
}

void ByteBuf::putBytes(uint8_t* b, uint32_t len, uint32_t index) {
	wpos = index;

	// Insert the data one byte at a time into the internal buffer at position i+starting index
	for (uint32_t i = 0; i < len; i++)
		append<uint8_t>(b[i]);
}

void ByteBuf::putChar(char value) {
	append<char>(value);
}

void ByteBuf::putChar(char value, uint32_t index) {
	insert<char>(value, index);
}

void ByteBuf::putDouble(double value) {
	append<double>(value);
}

void ByteBuf::putDouble(double value, uint32_t index) {
	insert<double>(value, index);
}
void ByteBuf::putFloat(float value) {
	append<float>(value);
}

void ByteBuf::putFloat(float value, uint32_t index) {
	insert<float>(value, index);
}

void ByteBuf::putInt(uint32_t value) {
	append<uint32_t>(value);
}

void ByteBuf::putInt(uint32_t value, uint32_t index) {
	insert<uint32_t>(value, index);
}

void ByteBuf::putLong(uint64_t value) {
	append<uint64_t>(value);
}

void ByteBuf::putLong(uint64_t value, uint32_t index) {
	insert<uint64_t>(value, index);
}

void ByteBuf::putShort(uint16_t value) {
	append<uint16_t>(value);
}

void ByteBuf::putShort(uint16_t value, uint32_t index) {
	insert<uint16_t>(value, index);
}
