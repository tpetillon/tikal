#pragma once

#include <functional>
#include <memory>
#include <type_traits>

#include <Hypodermic/ArgumentPack.h>
#include <Hypodermic/Container.h>
#include <Hypodermic/IsSupportedArgument.h>

#include "ComponentArgumentResolver.h"
#include "ComponentConstructorTypologyDeducer.h"

namespace tikal
{

class SceneObject;

namespace Traits
{

	namespace Details
	{

		template <class TParent>
		struct ComponentArgumentResolverInvoker
		{
			explicit ComponentArgumentResolverInvoker(Hypodermic::Container& container)
				: m_container(container)
			{
			}

			template <class T, class = typename std::enable_if< !std::is_convertible< TParent, T >::value && Hypodermic::Traits::IsSupportedArgument< typename std::decay< T >::type >::value >::type>
			operator T()
			{
				return ComponentArgumentResolver< typename std::decay< T >::type >::template resolve(m_container);
			}

		private:
			Hypodermic::Container& m_container;
		};


		template <class T, class TArgumentPack>
		struct ComponentConstructorDescriptor;

		template <class T>
		struct ComponentConstructorDescriptor< T, Hypodermic::Utils::ArgumentPack<> >
		{
			static std::function< T* (Hypodermic::Container&, void* const, SceneObject*) > describe()
			{
				return [](Hypodermic::Container& container, void* const placement, SceneObject* sceneObject)
				{
					return new (placement) T (sceneObject);
				};
			}
		};


		template <class T, class... TAnyArgument>
		struct ComponentConstructorDescriptor< T, Hypodermic::Utils::ArgumentPack< TAnyArgument... > >
		{
			static std::function< T* (Hypodermic::Container&, void* const, SceneObject*) > describe()
			{
				return [](Hypodermic::Container& container, void* const placement, SceneObject* sceneObject)
				{
					return new (placement) T (sceneObject, ComponentArgumentResolverInvoker< typename TAnyArgument::Type >(container)...);
				};
			}
		};

	}


	template <class T>
	using ComponentConstructorDescriptor = Details::ComponentConstructorDescriptor< T, Traits::ComponentConstructorTypologyDeducer< T > >;

	} // namespace Traits

}
