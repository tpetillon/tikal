#pragma once

#include <type_traits>
#include <utility>

#include "Hypodermic/ArgumentPack.h"
#include "Hypodermic/Config.h"
#include "Hypodermic/IntegerSequence.h"

#include "TemplateHelpers.h"

namespace tikal
{

class SceneObject;

namespace Traits
{

	struct CommandTypologyNotSupported
	{
		typedef CommandTypologyNotSupported Type;
	};


	namespace Details
	{

		// taken from https://stackoverflow.com/a/37161919
		template<class F, class... Args>
		struct is_callable
		{
			template<class U> static auto test(U* p) -> decltype((*p)(std::declval<Args>()...), void(), std::true_type());
			template<class U> static auto test(...) -> decltype(std::false_type());

			static constexpr bool value = decltype(test<F>(0))::value;
		};

		template <class, class, class, class = void>
		struct CommandTypologyDeducer;


		// Initial recursion state
		template <class T, class P>
		struct CommandTypologyDeducer
			<
			T,
			P,
			Hypodermic::Utils::IntegerSequence<>,
			typename std::enable_if
				<
					std::is_void<P>::value && is_callable< T >::value ||
					!std::is_void<P>::value && is_callable< T, P >::value
				>::type
			> : Hypodermic::Utils::ArgumentPack<>
		{};

		template <class T, class P>
		struct CommandTypologyDeducer
			<
			T,
			P,
			Hypodermic::Utils::IntegerSequence<>,
			typename std::enable_if
				<
					!(
						std::is_void<P>::value && is_callable< T >::value ||
						!std::is_void<P>::value && is_callable< T, P >::value
					)
				>::type
			> : CommandTypologyDeducer< T, P, Hypodermic::Utils::MakeIntegerSequence< 1 > >::Type
		{};


		// Common recusion state
		template <class T, class P, int... NthArgument>
		struct CommandTypologyDeducer
			<
			T,
			P,
			Hypodermic::Utils::IntegerSequence< NthArgument... >,
			typename std::enable_if
				<
					(Cardinality< NthArgument... >::value > 0 && Cardinality< NthArgument... >::value < HYPODERMIC_CONSTRUCTOR_ARGUMENT_COUNT) &&
					(
						std::is_void<P>::value && is_callable< T, WrapAndGet< T, NthArgument >... >::value ||
						!std::is_void<P>::value && is_callable< T, P, WrapAndGet< T, NthArgument >... >::value
					)
				>::type
			> : Hypodermic::Utils::ArgumentPack< WrapAndGet< T, NthArgument >... >
		{};

		template <class T, class P, int... NthArgument>
		struct CommandTypologyDeducer
			<
			T,
			P,
			Hypodermic::Utils::IntegerSequence< NthArgument... >,
			typename std::enable_if
				<
					(Cardinality< NthArgument... >::value > 0 && Cardinality< NthArgument... >::value < HYPODERMIC_CONSTRUCTOR_ARGUMENT_COUNT) &&
					!(
						std::is_void<P>::value && is_callable< T, WrapAndGet< T, NthArgument >... >::value ||
						!std::is_void<P>::value && is_callable< T, P, WrapAndGet< T, NthArgument >... >::value
					)
				>::type
			> : CommandTypologyDeducer< T, P, Hypodermic::Utils::MakeIntegerSequence< sizeof...(NthArgument)+1 > >::Type
		{};


		// Last recursion state
		template <class T, class P, int... NthArgument>
		struct CommandTypologyDeducer
			<
			T,
			P,
			Hypodermic::Utils::IntegerSequence< NthArgument... >,
			typename std::enable_if
				<
					(Cardinality< NthArgument... >::value == HYPODERMIC_CONSTRUCTOR_ARGUMENT_COUNT) &&
					(
						std::is_void<P>::value && is_callable< T, WrapAndGet< T, NthArgument >... >::value ||
						!std::is_void<P>::value && is_callable< T, P, WrapAndGet< T, NthArgument >... >::value
					)
				>::type
			> : Hypodermic::Utils::ArgumentPack< WrapAndGet< T, NthArgument >... >
		{};

		template <class T, class P, int... NthArgument>
		struct CommandTypologyDeducer
			<
			T,
			P,
			Hypodermic::Utils::IntegerSequence< NthArgument... >,
			typename std::enable_if
				<
					(Cardinality< NthArgument... >::value == HYPODERMIC_CONSTRUCTOR_ARGUMENT_COUNT) &&
					!(
						std::is_void<P>::value && is_callable< T, WrapAndGet< T, NthArgument >... >::value ||
						!std::is_void<P>::value && is_callable< T, P, WrapAndGet< T, NthArgument >... >::value
					)
				>::type
			> : CommandTypologyNotSupported
		{};

	} // namespace Details


	template <class T, class P>
	using CommandTypologyDeducer = typename Details::CommandTypologyDeducer< T, P, Hypodermic::Utils::MakeIntegerSequence< 0 > >::Type;

} // namespace Traits
} // namespace tikal
