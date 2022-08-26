#pragma once

#include "core/utility/base_wrapper/wrapper.hpp"
#include <variant>

namespace TwinKleS::Core {

	#pragma region type

	template <typename ...TValue> requires
		CategoryConstraint<IsPureInstance<TValue...>>
		&& (sizeof...(TValue) > 0_szz)
	class Variant {

	public: //

		using Value = TypePackage<TValue...>;

	protected: //

		std::variant<TValue...> m_value{};

	public: //

		#pragma region structor

		constexpr ~Variant (
		) = default;

		// ----------------

		constexpr Variant (
		) = default;

		constexpr Variant (
			Variant const & that
		) = default;

		constexpr Variant (
			Variant && that
		) = default;

		// ----------------

		template <typename ValueObject> requires
			CategoryConstraint<IsValid<ValueObject>>
			&& (IsSame<AsPure<ValueObject>, TValue...>)
		explicit constexpr Variant (
			ValueObject && value
		) :
			m_value{as_forward<ValueObject>(value)} {
		}

		#pragma endregion

		#pragma region operator

		constexpr auto operator = (
			Variant const & that
		) -> Variant& = default;

		constexpr auto operator = (
			Variant && that
		) -> Variant& = default;

		#pragma endregion

		#pragma region value

		template <typename ValueObject> requires
			CategoryConstraint<IsPureInstance<ValueObject>>
			&& (IsSame<ValueObject, TValue...>)
		constexpr auto is (
		) const -> Boolean {
			return std::holds_alternative<ValueObject>(thiz.m_value);
		}

		// ----------------

		template <typename ValueObject, typename ...Argument> requires
			CategoryConstraint<IsPureInstance<ValueObject> && IsValid<Argument...>>
			&& (IsSame<ValueObject, TValue...>)
			&& (IsConstructible<ValueObject, Argument &&...>)
		constexpr auto set (
			Argument && ...argument
		) -> ValueObject& {
			thiz.m_value.template emplace<ValueObject>(as_forward<Argument>(argument)...);
			return std::get<ValueObject>(thiz.m_value);
		}

		// ----------------

		template <typename ValueObject> requires
			CategoryConstraint<IsPureInstance<ValueObject>>
			&& (IsSame<ValueObject, TValue...>)
		constexpr auto get (
		) -> ValueObject& {
			assert_condition(thiz.template is<ValueObject>());
			return std::get<ValueObject>(thiz.m_value);
		}

		template <typename ValueObject> requires
			CategoryConstraint<IsPureInstance<ValueObject>>
			&& (IsSame<ValueObject, TValue...>)
		constexpr auto get (
		) const -> ValueObject const& {
			assert_condition(thiz.template is<ValueObject>());
			return std::get<ValueObject>(thiz.m_value);
		}

		#pragma endregion

		#pragma region index

		constexpr auto index (
		) const -> Size {
			return mbw<Size>(thiz.m_value.index());
		}

		#pragma endregion

		#pragma region value of index

		template <auto index> requires
			CategoryConstraint<>
			&& (IsSameV<index, ZSize>)
			&& (index < sizeof...(TValue))
		constexpr auto is_of_index (
		) const -> Boolean {
			return thiz.template is<AsSelect<index, TValue...>>();
		}

		// ----------------

		template <auto index, typename ...Argument> requires
			CategoryConstraint<IsValid<Argument...>>
			&& (IsSameV<index, ZSize>)
			&& (index < sizeof...(TValue))
			&& (IsConstructible<AsSelect<index, TValue...>, Argument &&...>)
		constexpr auto set_of_index (
			Argument && ...argument
		) -> AsSelect<index, TValue...>& {
			return thiz.template set<AsSelect<index, TValue...>>(as_forward<Argument>(argument)...);
		}

		// ----------------

		template <auto index> requires
			CategoryConstraint<>
			&& (IsSameV<index, ZSize>)
			&& (index < sizeof...(TValue))
		constexpr auto get_of_index (
		) -> AsSelect<index, TValue...>& {
			return thiz.template get<AsSelect<index, TValue...>>();
		}

		template <auto index> requires
			CategoryConstraint<>
			&& (IsSameV<index, ZSize>)
			&& (index < sizeof...(TValue))
		constexpr auto get_of_index (
		) const -> AsSelect<index, TValue...> const& {
			return thiz.template get<AsSelect<index, TValue...>>();
		}

		#pragma endregion

	public: //

		#pragma region operator

		friend constexpr auto operator == (
			Variant const & thix,
			Variant const & that
		) -> bool = default;

		#pragma endregion

	};

	#pragma endregion

	#pragma region utility

	template <typename ActiveValue, typename ...Value, typename ...Argument> requires
		CategoryConstraint<IsPureInstance<ActiveValue> && IsPureInstance<Value...> && IsValid<Argument...>>
	inline constexpr auto make_variant (
		Argument && ...argument
	) -> Variant<Value...> {
		auto result = Variant<Value...>{};
		result.template set<ActiveValue>(as_forward<Argument>(argument)...);
		return result;
	}

	template <typename ActiveValue, typename ...Value> requires
		CategoryConstraint<IsValid<ActiveValue> && IsPureInstance<Value...>>
	inline constexpr auto make_variant_of (
		ActiveValue && value
	) -> Variant<AsPure<Value>...> {
		return Variant<AsPure<Value>...>{as_forward<ActiveValue>(value)};
	}

	#pragma endregion

}
