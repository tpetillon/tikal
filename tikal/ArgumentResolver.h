#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <Hypodermic/Container.h>
#include <Hypodermic/IsComplete.h>
#include <Hypodermic/TypeInfo.h>

namespace tikal
{

namespace Traits
{

	template <class TArg>
	struct ArgumentResolver;


	template <class TArg>
	struct ArgumentResolver< std::shared_ptr< TArg > >
	{
		typedef std::shared_ptr< TArg > Type;

		static Type resolve(Hypodermic::Container* container)
		{
			static_assert(Hypodermic::Traits::IsComplete< TArg >::value, "TArg should be a complete type");

			return container->resolve< TArg >();
		}
	};

}

}
