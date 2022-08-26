#pragma once

#include "core/utility/utility.hpp"
#include "core/tool/common/version_utility.hpp"

namespace TwinKleS::Core::Tool::PopCap::PAM {

	struct Version {

		Integer number;

		// ----------------

		friend constexpr auto operator == (
			Version const & thix,
			Version const & that
		) -> bool = default;

	};

	using VersionEnum = ValuePackage<
		Version{1_i},
		Version{2_i},
		Version{3_i},
		Version{4_i},
		Version{5_i},
		Version{6_i}
	>;

	// ----------------

	inline constexpr auto check_version (
		Version const &                               it,
		std::initializer_list<Integer::Value> const & number
	) -> ZBoolean {
		auto result = true;
		result &= (number.size() == 0 || Range::has(number, it.number.value));
		return result;
	}

}

namespace TwinKleS::Core::JSON {

	M_json_value_adapter_for_aggregate_as_object_by_field_of_unique(
		M_wrap(Tool::PopCap::PAM::Version),
		M_wrap(number),
	);

}
