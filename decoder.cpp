#include "imgcomp/decoder.hpp"

#include <chrono>
#include <iostream>

namespace decoder {
	std::span<uint8_t> undo_rle(const std::span<uint8_t>& compressed_data) {
		uint8_t* new_data = new uint8_t[compressed_data.size() * 50]; // TODO: no random sizing

		size_t data_pos = 0;
		for (size_t i = 0; i < compressed_data.size(); i += 4) {
			uint8_t count = compressed_data[i + 3];

			for (int j = 0; j < count; j++) {
				memcpy(new_data + data_pos, compressed_data.data() + i, sizeof(uint8_t) * 3);
				data_pos += 3;
			}
		}

		return { new_data, data_pos };
	}

	std::span<uint8_t> decode_image(const std::span<uint8_t>& compressed_data, const uint8_t const* previous_data, const size_t previous_size) {
		if (!previous_data) {
			auto start = std::chrono::high_resolution_clock::now();

			auto raw_data = undo_rle(compressed_data);

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			std::cout << "\n\nundo rle: " << duration << "ms " << compressed_data.size() / 1024.f / 1024.f << "mb -> " << raw_data.size() / 1024.f / 1024.f << "mb" << std::endl;

			size_t new_size = (raw_data.size() / 3) * 4;
			uint8_t* data = new uint8_t[new_size];

			start = std::chrono::high_resolution_clock::now();

			for (size_t i = 0; i < raw_data.size() / 3; i++) {
				memcpy(data + i * 4, raw_data.data() + i * 3, sizeof(uint8_t) * 3);
				// set alpha
				data[i * 4 + 3] = 255;
			}

			end = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			delete[] raw_data.data();

			std::cout << "convert to bitmap: " << duration << "ms " << raw_data.size() / 1024.f / 1024.f << "mb -> " << new_size / 1024.f / 1024.f << "mb" << std::endl;

			return { data, new_size };
		}

		else {
			uint8_t* data = new uint8_t[previous_size];
			memcpy(data, previous_data, previous_size);

			for (size_t i = 0; i < compressed_data.size(); i += 4) {
				uint32_t position = *reinterpret_cast<uint32_t*>(compressed_data.data() + i);
				i += sizeof(uint32_t);

				uint8_t count = compressed_data[i + 3];

				for (uint8_t j = 0; j < count; j++)
					memcpy(data + position * 4 + j * 4, compressed_data.data() + i, 3);
			}

			return { data, previous_size };
		}
	}
}