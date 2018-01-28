#pragma once

#include "Hypodermic/AnyArgument.h"

namespace tikal
{

namespace Traits
{

	namespace Details
	{
		template <int... N>
		struct Cardinality
		{
			static const int value = sizeof...(N);
		};

		template <class T, int>
		struct WrapAndGet : Hypodermic::Traits::AnyArgument< T > {};
	}

}

}