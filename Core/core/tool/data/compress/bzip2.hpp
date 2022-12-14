#pragma once

#include "core/utility/utility.hpp"
#include "core/third_party/bzip2.hpp"

namespace TwinStar::Core::Tool::Data::Compress::BZip2 {

	struct CompressCommon {

	protected:

	};

	struct Compress :
		CompressCommon {

	protected:

		static auto process_whole (
			IByteStreamView & raw,
			OByteStreamView & ripe,
			Size const &      block_size,
			Size const &      work_factor
		) -> Void {
			assert_condition(Math::between(block_size, 1_sz, 9_sz));
			assert_condition(Math::between(work_factor, 0_sz, 250_sz));
			auto bz_stream = ThirdParty::bzip2::bz_stream{
				.next_in = cast_pointer<char>(as_variable_pointer(raw.current_pointer())).value,
				.avail_in = static_cast<unsigned int>(raw.reserve().value),
				.total_in_lo32 = 0,
				.total_in_hi32 = 0,
				.next_out = cast_pointer<char>(ripe.current_pointer()).value,
				.avail_out = static_cast<unsigned int>(ripe.reserve().value),
				.total_out_lo32 = 0,
				.total_out_hi32 = 0,
				.state = nullptr,
				.bzalloc = nullptr,
				.bzfree = nullptr,
				.opaque = nullptr,
			};
			auto state = int{};
			state = ThirdParty::bzip2::BZ2_bzCompressInit(
				&bz_stream,
				static_cast<int>(block_size.value),
				0,
				static_cast<int>(work_factor.value)
			);
			assert_condition(state == ThirdParty::bzip2::BZ_OK_);
			state = ThirdParty::bzip2::BZ2_bzCompress(
				&bz_stream,
				ThirdParty::bzip2::BZ_FINISH_
			);
			assert_condition(state == ThirdParty::bzip2::BZ_STREAM_END_);
			state = ThirdParty::bzip2::BZ2_bzCompressEnd(
				&bz_stream
			);
			assert_condition(state == ThirdParty::bzip2::BZ_OK_);
			assert_condition(bz_stream.avail_in == 0);
			raw.forward(mbw<Size>((static_cast<uint64_t>(bz_stream.total_in_hi32) << 32) + static_cast<uint64_t>(bz_stream.total_in_lo32)));
			ripe.forward(mbw<Size>((static_cast<uint64_t>(bz_stream.total_out_hi32) << 32) + static_cast<uint64_t>(bz_stream.total_out_lo32)));
			assert_condition(raw.full());
			return;
		}

	public:

		static auto do_process_whole (
			IByteStreamView & raw_,
			OByteStreamView & ripe_,
			Size const &      block_size,
			Size const &      work_factor
		) -> Void {
			M_use_zps_of(raw);
			M_use_zps_of(ripe);
			return process_whole(raw, ripe, block_size, work_factor);
		}

	};

	struct Uncompress :
		CompressCommon {

	protected:

		static auto process_whole (
			IByteStreamView & ripe,
			OByteStreamView & raw,
			Boolean const &   small
		) -> Void {
			auto bz_stream = ThirdParty::bzip2::bz_stream{
				.next_in = cast_pointer<char>(as_variable_pointer(ripe.current_pointer())).value,
				.avail_in = static_cast<unsigned int>(ripe.reserve().value),
				.total_in_lo32 = 0,
				.total_in_hi32 = 0,
				.next_out = cast_pointer<char>(raw.current_pointer()).value,
				.avail_out = static_cast<unsigned int>(raw.reserve().value),
				.total_out_lo32 = 0,
				.total_out_hi32 = 0,
				.state = nullptr,
				.bzalloc = nullptr,
				.bzfree = nullptr,
				.opaque = nullptr,
			};
			auto state = int{};
			state = ThirdParty::bzip2::BZ2_bzDecompressInit(
				&bz_stream,
				0,
				static_cast<int>(small.value)
			);
			assert_condition(state == ThirdParty::bzip2::BZ_OK_);
			state = ThirdParty::bzip2::BZ2_bzDecompress(
				&bz_stream
			);
			assert_condition(state == ThirdParty::bzip2::BZ_STREAM_END_);
			state = ThirdParty::bzip2::BZ2_bzDecompressEnd(
				&bz_stream
			);
			assert_condition(state == ThirdParty::bzip2::BZ_OK_);
			ripe.forward(mbw<Size>((static_cast<uint64_t>(bz_stream.total_in_hi32) << 32) + static_cast<uint64_t>(bz_stream.total_in_lo32)));
			raw.forward(mbw<Size>((static_cast<uint64_t>(bz_stream.total_out_hi32) << 32) + static_cast<uint64_t>(bz_stream.total_out_lo32)));
			return;
		}

	public:

		static auto do_process_whole (
			IByteStreamView & ripe_,
			OByteStreamView & raw_,
			Boolean const &   small
		) -> Void {
			M_use_zps_of(ripe);
			M_use_zps_of(raw);
			return process_whole(ripe, raw, small);
		}

	};

}
