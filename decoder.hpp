#ifndef _DECODER_H_
#define _DECODER_H_

#include <cstdint>
#include <span>

namespace decoder {
	extern std::span<uint8_t> decode_image(const std::span<uint8_t>& compressed_data, const uint8_t const* previous_data = nullptr, const size_t previous_size = 0);
}

#endif