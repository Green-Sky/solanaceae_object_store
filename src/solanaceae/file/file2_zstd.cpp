#include "./file2_zstd.hpp"

#include <cstdint>
#include <iostream>
#include <variant>
#include <vector>

#include <cassert>

File2ZSTDW::File2ZSTDW(File2I& real) :
	File2I(true, false),
	_real_file(real)
{
}

File2ZSTDW::~File2ZSTDW(void) {
	// flush remaining data (and maybe header)
	// actually nvm, write will always flush all data, so only on empty files this would be an issue
}

bool File2ZSTDW::isGood(void) {
	return _real_file.isGood();
}

bool File2ZSTDW::write(const ByteSpan data, int64_t pos) {
	if (pos != -1) {
		return false;
	}

	if (data.empty()) {
		return false; // return true?
	}

	if (data.size < 16) {
		std::cout << "F2ZSTD warning: each write is a zstd frame and compression suffers significantly for small frames.\n";
	}

	std::vector<uint8_t> compressed_buffer(ZSTD_compressBound(data.size));
	const auto ret = ZSTD_compress(compressed_buffer.data(), compressed_buffer.size(), data.ptr, data.size, 0);

	if (ZSTD_isError(ret)) {
		std::cerr << "F2WRZSTD error: compressing data failed\n";
		return false;
	}

	return _real_file.write(ByteSpan{compressed_buffer.data(), ret}) && _real_file.isGood();
}

ByteSpanWithOwnership File2ZSTDW::read(uint64_t, int64_t) {
	return ByteSpan{};
}

// ######################################### decompression

File2ZSTDR::File2ZSTDR(File2I& real) :
	File2I(false, true),
	_real_file(real),

	// 64kib
	_in_buffer(ZSTD_DStreamInSize()),
	_out_buffer(ZSTD_DStreamOutSize())
{
}

bool File2ZSTDR::isGood(void) {
	return _real_file.isGood();
}

bool File2ZSTDR::write(const ByteSpan, int64_t) {
	return false;
}

ByteSpanWithOwnership File2ZSTDR::read(uint64_t size, int64_t pos) {
	if (pos != -1) {
		// error, only support streaming (for now)
		return ByteSpan{};
	}

	std::vector<uint8_t> ret_data;

	// actually first we check previous data
	if (!_decompressed_buffer.empty()) {
		uint64_t required_size = std::min<uint64_t>(size, _decompressed_buffer.size());
		ret_data.insert(ret_data.end(), _decompressed_buffer.cbegin(), _decompressed_buffer.cbegin() + required_size);
		_decompressed_buffer.erase(_decompressed_buffer.cbegin(), _decompressed_buffer.cbegin() + required_size);
	}

	bool eof {false};
	// outerloop here
	while (ret_data.size() < size && !eof) {
		// first make sure we have data in input
		if (_z_input.src == nullptr || _z_input.pos == _z_input.size) {
			const auto request_size = _in_buffer.size();
			if (!feedInput(_real_file.read(request_size, -1))) {
				return ByteSpanWithOwnership{std::move(ret_data)};
			}

			// if _z_input.size < _in_buffer.size() -> assume eof?
			if (_z_input.size < request_size) {
				eof = true;
				//std::cout << "---- eof\n";
			}
		}

		do {
			ZSTD_outBuffer output = { _out_buffer.data(), _out_buffer.size(), 0 };
			size_t const ret = ZSTD_decompressStream(_dctx.get(), &output , &_z_input);
			if (ZSTD_isError(ret)) {
				// error <.<
				std::cerr << "---- error: decompression error\n";
				return ByteSpanWithOwnership{std::move(ret_data)};
			}

			// no new decomp data?
			if (output.pos == 0) {
				if (ret != 0) {
					// if not error and not 0, indicates that
					// there is additional flushing needed
					continue;
				}

				assert(eof || ret == 0);
				break;
			}

			int64_t returning_size = std::min<int64_t>(int64_t(size) - int64_t(ret_data.size()), output.pos);
			assert(returning_size >= 0);
			if (returning_size > 0) {
				ret_data.insert(
					ret_data.end(),
					reinterpret_cast<const uint8_t*>(output.dst),
					reinterpret_cast<const uint8_t*>(output.dst) + returning_size
				);
			}

			// make sure we keep excess decompressed data
			if (returning_size < int64_t(output.pos)) {
				//const auto remaining_size = output.pos - returning_size;
				_decompressed_buffer.insert(
					_decompressed_buffer.cend(),
					reinterpret_cast<const uint8_t*>(output.dst) + returning_size,
					reinterpret_cast<const uint8_t*>(output.dst) + output.pos
				);
			}
		} while (_z_input.pos < _z_input.size);
	}

	return ByteSpanWithOwnership{std::move(ret_data)};
}

bool File2ZSTDR::feedInput(std::variant<ByteSpan, std::vector<uint8_t>>&& read_buff) {
	// TODO: optimize, we copy the buffer, but we might not need to
	if (std::holds_alternative<ByteSpan>(read_buff)) {
		const auto& span = std::get<ByteSpan>(read_buff);
		if (span.size > _in_buffer.size()) {
			// error, how did we read more than we asked for??
			return false;
		}

		if (span.empty()) {
			_z_input = { _in_buffer.data(), 0, 0 };
		} else {
			// cpy
			_in_buffer = static_cast<std::vector<uint8_t>>(span);
			_z_input = {
				_in_buffer.data(),
				span.size,
				0
			};
		}
	} else if (std::holds_alternative<std::vector<uint8_t>>(read_buff)) {
		auto& vec = std::get<std::vector<uint8_t>>(read_buff);
		if (vec.size() > _in_buffer.size()) {
			// error, how did we read more than we asked for??
			return false;
		}

		// cpy
		_in_buffer = vec;

		_z_input = {
			_in_buffer.data(),
			_in_buffer.size(),
			0
		};
	} else {
		// error, unsupported return value of read??
		return false;
	}

	return true;
}

