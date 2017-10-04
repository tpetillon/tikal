#pragma once

#include <type_traits>

#include "Hypodermic/AnyArgument.h"
#include "Hypodermic/ArgumentPack.h"
#include "Hypodermic/Config.h"
#include "Hypodermic/IntegerSequence.h"


namespace tikal
{

class SceneObject;

namespace Traits
{

	struct ConstructorTypologyNotSupported
	{
		typedef ConstructorTypologyNotSupported Type;
	};


	namespace Details
	{

		template <int... N>
		struct Cardinality
		{
			static const int value = sizeof...(N);
		};

		template <class T, int>
		struct WrapAndGet : Hypodermic::Traits::AnyArgument< T > {};

		template <class, class, class = void>
		struct ComponentConstructorTypologyDeducer;


		// Initial recursion state
		template <class T>
		struct ComponentConstructorTypologyDeducer
			<
			T,
			Hypodermic::Utils::IntegerSequence<>,
			typename std::enable_if< std::is_constructible< T, SceneObject* const >::value >::type
				> : Hypodermic::Utils::ArgumentPack<>
		{};

		template <class T>
		struct ComponentConstructorTypologyDeducer
			<
			T,
			Hypodermic::Utils::IntegerSequence<>,
			typename std::enable_if< !std::is_constructible< T, SceneObject* const >::value >::type
			> : ComponentConstructorTypologyDeducer< T, Hypodermic::Utils::MakeIntegerSequence< 1 > >::Type
		{};


		// Common recusion state
		template <class T, int... NthArgument>
		struct ComponentConstructorTypologyDeducer
			<
			T,
			Hypodermic::Utils::IntegerSequence< NthArgument... >,
			typename std::enable_if
			<
			(Cardinality< NthArgument... >::value > 0 && Cardinality< NthArgument... >::value < HYPODERMIC_CONSTRUCTOR_ARGUMENT_COUNT) &&
			std::is_constructible< T, SceneObject* const, WrapAndGet< T, NthArgument >... >::value
			>::type
				> : Hypodermic::Utils::ArgumentPack< WrapAndGet< T, NthArgument >... >
		{};

		template <class T, int... NthArgument>
		struct ComponentConstructorTypologyDeducer
			<
			T,
			Hypodermic::Utils::IntegerSequence< NthArgument... >,
			typename std::enable_if
			<
			(Cardinality< NthArgument... >::value > 0 && Cardinality< NthArgument... >::value < HYPODERMIC_CONSTRUCTOR_ARGUMENT_COUNT) &&
			!std::is_constructible< T, SceneObject* const, WrapAndGet< T, NthArgument >... >::value
			>::type
			> : ComponentConstructorTypologyDeducer< T, Hypodermic::Utils::MakeIntegerSequence< sizeof...(NthArgument)+1 > >::Type
		{};


		// Last recursion state
		template <class T, int... NthArgument>
		struct ComponentConstructorTypologyDeducer
			<
			T,
			Hypodermic::Utils::IntegerSequence< NthArgument... >,
			typename std::enable_if< (Cardinality< NthArgument... >::value == HYPODERMIC_CONSTRUCTOR_ARGUMENT_COUNT) && std::is_constructible< T, SceneObject* const, WrapAndGet< T, NthArgument >... >::value >::type
				> : Hypodermic::Utils::ArgumentPack< WrapAndGet< T, NthArgument >... >
		{};

		template <class T, int... NthArgument>
		struct ComponentConstructorTypologyDeducer
			<
			T,
			Hypodermic::Utils::IntegerSequence< NthArgument... >,
			typename std::enable_if< (Cardinality< NthArgument... >::value == HYPODERMIC_CONSTRUCTOR_ARGUMENT_COUNT) && !std::is_constructible< T, SceneObject* const, WrapAndGet< T, NthArgument >... >::value >::type
			> : ComponentConstructorTypologyNotSupported
		{};

	} // namespace Details


	template <class T>
	using ComponentConstructorTypologyDeducer = typename Details::ComponentConstructorTypologyDeducer< T, Hypodermic::Utils::MakeIntegerSequence< 0 > >::Type;

} // namespace Traits
} // namespace tikal
