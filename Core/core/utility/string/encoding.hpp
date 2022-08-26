#pragma once

#include "core/utility/string/string.hpp"

#include <locale>
#include <codecvt>

namespace TwinKleS::Core::StringEncoding {

	#pragma region utf-8 & utf-16

	inline auto utf8_to_utf16 (
		CBasicStringView<Character8> const & source
	) -> BasicString<Character16> {
		auto converter = std::wstring_convert<std::codecvt_utf8_utf16<ZCharacter16>, ZCharacter16>{};
		auto result = converter.from_bytes(
			cast_pointer<ZCharacterLocale>(source.begin()).value,
			cast_pointer<ZCharacterLocale>(source.end()).value
		);
		assert_condition(converter.converted() == source.size().value);
		return BasicString<Character16>{cast_pointer<Character16>(make_pointer(result.data())), mbw<Size>(result.size())};
	}

	inline auto utf16_to_utf8 (
		CBasicStringView<Character16> const & source
	) -> BasicString<Character8> {
		auto converter = std::wstring_convert<std::codecvt_utf8_utf16<ZCharacter16>, ZCharacter16>{};
		auto result = converter.to_bytes(
			cast_pointer<ZCharacter16>(source.begin()).value,
			cast_pointer<ZCharacter16>(source.end()).value
		);
		assert_condition(converter.converted() == source.size().value);
		return BasicString<Character8>{cast_pointer<Character8>(make_pointer(result.data())), mbw<Size>(result.size())};
	}

	#pragma endregion

}
