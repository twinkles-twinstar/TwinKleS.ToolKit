#pragma once

#include "core/utility/data/json/value.hpp"
#include "core/utility/misc/2d_type.hpp"

namespace TwinKleS::Core::Image {

	#pragma region type

	using ImageSize = Size2D<Size>;

	using ImagePosition = Position2D<Size>;

	#pragma endregion

	#pragma region constant

	inline constexpr auto k_begin_image_position = ImagePosition{k_begin_index, k_begin_index};

	inline constexpr auto k_none_image_size = ImageSize{k_none_size, k_none_size};

	#pragma endregion

}
