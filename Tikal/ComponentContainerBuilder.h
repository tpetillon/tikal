#pragma once

#include "Component.h"
#include "ComponentConstructorDescriptor.h"
#include "ComponentContainer.h"
#include "ComponentRegistration.h"

#include <Hypodermic/Container.h>
#include <Hypodermic/IsComplete.h>
#include <Hypodermic/TypeInfo.h>

#include <functional>
#include <list>
#include <memory>

namespace Hypodermic
{
class Container;
}

namespace tikal
{

class ComponentContainerBuilder
{
public:
	template<typename TComponent>
	void registerComponent()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "TComponent must derive from Component");
		static_assert(Hypodermic::Traits::IsComplete<TComponent>::value, "TComponent should be a complete type");

		auto&& factory = Traits::ComponentConstructorDescriptor<TComponent>::describe();

		m_registrations.push_back(std::make_shared<ComponentRegistration>(
			Hypodermic::Utils::getMetaTypeInfo<TComponent>(),
			[factory](Hypodermic::Container& container, void* const placement)
			{
				return static_cast<void*>(factory(container, placement));
			}
		));
	}

	std::shared_ptr<ComponentContainer> build(
		std::shared_ptr<Hypodermic::Container> container)
	{
		return std::make_shared<ComponentContainer>(container, m_registrations);
	}

private:
	std::list<std::shared_ptr<ComponentRegistration>> m_registrations;
};

}
