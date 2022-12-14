#pragma once

#include "core/utility/utility.hpp"
#include "core/third_party/PVRTCCompressor.hpp"

namespace TwinStar::Core::Tool::Texture::Compress::PVRTC4 {

	struct CompressCommon {

	protected:

		inline static constexpr auto k_bpp = Size{4_sz};

		inline static constexpr auto k_block_width = Size{4_sz};

	};

	struct Compress :
		CompressCommon {

	protected:

		static auto process_image_rgb (
			OByteStreamView &          data,
			Image::CBitmapView const & image
		) -> Void {
			assert_condition(is_padded_size(image.size().width, k_block_width));
			assert_condition(is_padded_size(image.size().height, k_block_width));
			auto actual_image = ThirdParty::PVRTCCompressor::RgbBitmap{static_cast<int>(image.size().width.value), static_cast<int>(image.size().height.value)};
			auto actual_data = VListView<ThirdParty::PVRTCCompressor::ColorRgb<unsigned char>>{make_pointer(actual_image.GetData()), image.size().area()};
			for (auto & y : SizeRange{image.size().height}) {
				for (auto & x : SizeRange{image.size().width}) {
					auto & actual_pixel = actual_data[y * image.size().width + x];
					auto & pixel = image[y][x];
					actual_pixel.r = pixel.red.value;
					actual_pixel.g = pixel.green.value;
					actual_pixel.b = pixel.blue.value;
				}
			}
			ThirdParty::PVRTCCompressor::PvrTcEncoder::EncodeRgb4Bpp(data.current_pointer().value, actual_image);
			data.forward(image.size().area() * k_bpp / k_type_bit_count<Byte>);
			return;
		}

		static auto process_image_rgba (
			OByteStreamView &          data,
			Image::CBitmapView const & image
		) -> Void {
			assert_condition(is_padded_size(image.size().width, k_block_width));
			assert_condition(is_padded_size(image.size().height, k_block_width));
			auto actual_image = ThirdParty::PVRTCCompressor::RgbaBitmap{static_cast<int>(image.size().width.value), static_cast<int>(image.size().height.value)};
			auto actual_data = VListView<ThirdParty::PVRTCCompressor::ColorRgba<unsigned char>>{make_pointer(actual_image.GetData()), image.size().area()};
			for (auto & y : SizeRange{image.size().height}) {
				for (auto & x : SizeRange{image.size().width}) {
					auto & actual_pixel = actual_data[y * image.size().width + x];
					auto & pixel = image[y][x];
					actual_pixel.r = pixel.red.value;
					actual_pixel.g = pixel.green.value;
					actual_pixel.b = pixel.blue.value;
					actual_pixel.a = pixel.alpha.value;
				}
			}
			ThirdParty::PVRTCCompressor::PvrTcEncoder::EncodeRgba4Bpp(data.current_pointer().value, actual_image);
			data.forward(image.size().area() * k_bpp / k_type_bit_count<Byte>);
			return;
		}

		// ----------------

		static auto process_image (
			OByteStreamView &          data,
			Image::CBitmapView const & image,
			Boolean const &            with_alpha
		) -> Void {
			if (with_alpha) {
				process_image_rgba(data, image);
			} else {
				process_image_rgb(data, image);
			}
			return;
		}

	public:

		static auto do_process_image (
			OByteStreamView &          data_,
			Image::CBitmapView const & image,
			Boolean const &            with_alpha
		) -> Void {
			M_use_zps_of(data);
			return process_image(data, image, with_alpha);
		}

	};

	struct Uncompress :
		CompressCommon {

	protected:

		static auto process_image_rgb (
			IByteStreamView &          data,
			Image::VBitmapView const & image
		) -> Void {
			assert_condition(is_padded_size(image.size().width, k_block_width));
			assert_condition(is_padded_size(image.size().height, k_block_width));
			auto actual_data = Array<ThirdParty::PVRTCCompressor::ColorRgb<unsigned char>>{image.size().area()};
			ThirdParty::PVRTCCompressor::PvrTcDecoder::DecodeRgb4Bpp(actual_data.begin().value, ThirdParty::PVRTCCompressor::Point2<int>{static_cast<int>(image.size().width.value), static_cast<int>(image.size().height.value)}, data.current_pointer().value);
			for (auto & y : SizeRange{image.size().height}) {
				for (auto & x : SizeRange{image.size().width}) {
					auto & actual_pixel = actual_data[y * image.size().width + x];
					auto & pixel = image[y][x];
					pixel.red = Image::Channel{actual_pixel.r};
					pixel.green = Image::Channel{actual_pixel.g};
					pixel.blue = Image::Channel{actual_pixel.b};
				}
			}
			data.forward(image.size().area() * k_bpp / k_type_bit_count<Byte>);
			return;
		}

		static auto process_image_rgba (
			IByteStreamView &          data,
			Image::VBitmapView const & image
		) -> Void {
			assert_condition(is_padded_size(image.size().width, k_block_width));
			assert_condition(is_padded_size(image.size().height, k_block_width));
			auto actual_data = Array<ThirdParty::PVRTCCompressor::ColorRgba<unsigned char>>{image.size().area()};
			ThirdParty::PVRTCCompressor::PvrTcDecoder::DecodeRgba4Bpp(actual_data.begin().value, ThirdParty::PVRTCCompressor::Point2<int>{static_cast<int>(image.size().width.value), static_cast<int>(image.size().height.value)}, data.current_pointer().value);
			for (auto & y : SizeRange{image.size().height}) {
				for (auto & x : SizeRange{image.size().width}) {
					auto & actual_pixel = actual_data[y * image.size().width + x];
					auto & pixel = image[y][x];
					pixel.red = Image::Channel{actual_pixel.r};
					pixel.green = Image::Channel{actual_pixel.g};
					pixel.blue = Image::Channel{actual_pixel.b};
					pixel.alpha = Image::Channel{actual_pixel.a};
				}
			}
			data.forward(image.size().area() * k_bpp / k_type_bit_count<Byte>);
			return;
		}

		// ----------------

		static auto process_image (
			IByteStreamView &          data,
			Image::VBitmapView const & image,
			Boolean const &            with_alpha
		) -> Void {
			if (with_alpha) {
				process_image_rgba(data, image);
			} else {
				process_image_rgb(data, image);
			}
			return;
		}

	public:

		static auto do_process_image (
			IByteStreamView &          data_,
			Image::VBitmapView const & image,
			Boolean const &            with_alpha
		) -> Void {
			M_use_zps_of(data);
			// TODO : reset image
			return process_image(data, image, with_alpha);
		}

	};

}
