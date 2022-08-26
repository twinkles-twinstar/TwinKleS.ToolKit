#pragma once

#include "core/utility/base_wrapper/wrapper.hpp"
#include <bitset>

namespace TwinKleS::Core {

	#pragma region type

	template <auto t_size> requires
		CategoryConstraint<>
		&& (IsSameV<t_size, ZSize>)
	class BitSet {

	public: //

		using BoundedInteger = decltype([] {
			if constexpr (t_size <= 0_szz) {
				return;
			} else if constexpr (t_size <= 8_szz) {
				return IntegerU8{};
			} else if constexpr (t_size <= 16_szz) {
				return IntegerU16{};
			} else if constexpr (t_size <= 32_szz) {
				return IntegerU32{};
			} else if constexpr (t_size <= 64_szz) {
				return IntegerU64{};
			} else {
				return;
			}
		}());

	protected: //

		std::bitset<t_size> m_data;

	public: //

		#pragma region structor

		constexpr ~BitSet (
		) = default;

		// ----------------

		constexpr BitSet (
		) = default;

		constexpr BitSet (
			BitSet const & that
		) = default;

		constexpr BitSet (
			BitSet && that
		) = default;

		#pragma endregion

		#pragma region operator

		constexpr auto operator = (
			BitSet const & that
		) -> BitSet& = default;

		constexpr auto operator = (
			BitSet && that
		) -> BitSet& = default;

		#pragma endregion

		#pragma region size

		auto size (
		) const -> Size {
			return mbw<Size>(thiz.m_data.size());
		}

		#pragma endregion

		#pragma region value

		auto get (
			Size const & index
		) -> Boolean {
			return thiz.m_data.test(index.value);
		}

		auto set (
			Size const &    index,
			Boolean const & value
		) -> Void {
			thiz.m_data.set(index.value, value.value);
			return;
		}

		// ----------------

		auto reset (
		) -> Void {
			thiz.m_data.reset();
			return;
		}

		auto reset (
			Size const & index
		) -> Void {
			thiz.m_data.reset(index.value);
			return;
		}

		// ----------------

		auto set (
		) -> Void {
			thiz.m_data.set();
			return;
		}

		auto set (
			Size const & index
		) -> Void {
			thiz.m_data.set(index.value);
			return;
		}

		// ----------------

		#pragma endregion

		#pragma region utility

		auto all (
		) const -> Boolean {
			return mbw<Boolean>(thiz.m_data.all());
		}

		auto any (
		) const -> Boolean {
			return mbw<Boolean>(thiz.m_data.any());
		}

		auto none (
		) const -> Boolean {
			return mbw<Boolean>(thiz.m_data.none());
		}

		// ----------------

		auto count (
		) const -> Size {
			return mbw<Size>(thiz.m_data.count());
		}

		// ----------------

		auto flip (
		) -> Void {
			thiz.m_data.flip();
			return;
		}

		#pragma endregion

		#pragma region from & to with integer

		template <typename TargetInteger = BoundedInteger> requires
			NoneConstraint
			&& (IsSame<TargetInteger, BoundedInteger>)
			&& (!IsVoid<TargetInteger>)
		auto from_integer (
			TargetInteger const & value
		) -> Void {
			thiz.m_data = std::bitset<t_size>{value.value};
			return;
		}

		template <typename TargetInteger = BoundedInteger> requires
			NoneConstraint
			&& (IsSame<TargetInteger, BoundedInteger>)
			&& (!IsVoid<TargetInteger>)
		auto to_integer (
		) -> TargetInteger {
			return mbw<TargetInteger>(thiz.m_data.to_ullong());
		}

		#pragma endregion

	};

	#pragma endregion

}
