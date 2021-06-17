#include "imgcomp/encoder.hpp"
#include "imgcomp/capture.hpp"

#include <iostream>
#include <chrono>

namespace encoder {
	std::span<uint8_t> do_rle(const std::span<uint8_t>& data) {
		uint8_t* new_data = new uint8_t[data.size() / 3 * 4];

		size_t data_pos = 0;
		for (size_t i = 0; i < data.size();) {
			uint8_t count = 1;

			while (i + count * 3 < data.size() && count < 255) {
				// check if the next pixel is the same as this one
				if (data[i] < data[i + count * 3] - 5 || data[i] > data[i + count * 3] + 5)
					break;
				if (data[i + 1] < data[i + count * 3 + 1] - 5 || data[i + 1] > data[i + count * 3 + 1] + 5)
					break;
				if (data[i + 2] < data[i + count * 3 + 2] - 5 || data[i + 2] > data[i + count * 3 + 2] + 5)
					break;

				count++;
			}

			memcpy(new_data + data_pos, data.data() + i, sizeof(uint8_t) * 3);
			new_data[data_pos + 3] = count;
			data_pos += 4;

			i += count * 3;
		}

		return { new_data, data_pos };
	}

	std::span<uint8_t> bitmap_to_raw(uint8_t* data, size_t width, size_t height) {
		constexpr int shrink_factor = 2;
		constexpr int div = shrink_factor * shrink_factor;

		// vector for new image data
		size_t new_size = (width / shrink_factor) * (height / shrink_factor) * 3;
		//std::vector<uint8_t> new_data(new_size);
		uint8_t* new_data = new uint8_t[new_size];

		// int array for averaging pixels
		uint32_t totals[3];

		for (size_t y = 0; y < height; y += shrink_factor)
		{
			for (size_t x = 0; x < width; x += shrink_factor)
			{
				memset(totals, 0, sizeof(uint32_t) * 3);
				// average the surrounding pixels
				for (size_t xo = 0; xo < shrink_factor; xo++) {
					for (size_t yo = 0; yo < shrink_factor; yo++) {
						size_t row = height - y - yo - 1;
						size_t index = (row * width + x + xo) * 4;
						totals[0] += data[index];
						totals[1] += data[index + 1];
						totals[2] += data[index + 2];
					}
				}

				size_t new_row = height / shrink_factor - y / shrink_factor - 1;
				size_t new_index = (new_row * width / shrink_factor + x / shrink_factor) * 3;

				// update with average for area
				new_data[new_index] = totals[0] / div;
				new_data[++new_index] = totals[1] / div;
				new_data[++new_index] = totals[2] / div;
			}
		}

		// return a span with array pointer and size
		return { new_data, new_size };
	}

	std::span<uint8_t> encode_raw(const std::span<uint8_t>& raw, std::span<uint8_t> old_raw) {
		if (!old_raw.data()) {
			auto rle_data = do_rle(raw);

			return rle_data;
		}

		else {
			int guess_size = raw.size() / 5;
			uint8_t* data = new uint8_t[guess_size];
			size_t data_position = 0;

			for (size_t i = 0; i < raw.size(); i += 3) {
				// if the pixel is similar, skip
				if (raw[i] + 5 > old_raw[i] && raw[i] - 5 < old_raw[i])
					if (raw[i + 1] + 5 > old_raw[i + 1] && raw[i + 1] - 5 < old_raw[i + 1])
						if (raw[i + 2] + 5 > old_raw[i + 2] && raw[i + 2] - 5 < old_raw[i + 2])
							continue;

				uint8_t count = 1;

				while (i + count * 3 < raw.size() && count < 255) {
					if (raw[i] - 5 < old_raw[i] || raw[i] + 5 > old_raw[i])
						break;
					if (raw[i + 1] - 5 < old_raw[i + 1] || raw[i + 1] + 5 > old_raw[i + 1])
						break;
					if (raw[i + 2] - 5 < old_raw[i + 2] || raw[i + 2] + 5 > old_raw[i + 2])
						break;

					count++;
				}

				if (data_position > guess_size)
					return { static_cast<uint8_t*>(nullptr), INT_MAX };

				uint32_t position = static_cast<uint32_t>(i / 3);
				memcpy(data + data_position, &position, sizeof(uint32_t));
				data_position += sizeof(uint32_t);

				// copy rgb data
				memcpy(data + data_position, raw.data() + i, 3);

				data[data_position + 3] = count;

				data_position += 4;
			}

			return { data, data_position };
		}
	}
}