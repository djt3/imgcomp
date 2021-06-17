#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <cstdint>
#include <wtypes.h>
#include <vector>
#include <span>

namespace encoder {
	// takes a bitmap and returns a compressed byte array
	extern std::span<uint8_t> encode_raw(const std::span<uint8_t>& raw, std::span<uint8_t> old_raw = std::span<uint8_t>(static_cast<uint8_t*>(nullptr), 0));

	// takes raw bitmap data, removes alpha channel and scales
	extern std::span<uint8_t> bitmap_to_raw(uint8_t* data, size_t width, size_t height);
}

#endif