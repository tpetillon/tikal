#pragma once

#include "ComponentContainer.h"
#include "ComponentRepository.h"

#include <memory>

namespace Hypodermic
{
class Container;
}

namespace tikal
{

class ComponentInstantiator
{
public:
	ComponentInstantiator(
		std::shared_ptr<ComponentContainer> container,
		std::shared_ptr<ComponentRepository> m_componentRepository);

	template<typename TComponent>
	TComponent* instantiate()
	{
		static_assert(std::is_base_of<Component, TComponent>::value, "TComponent must derive from Component");

		return m_componentRepository->createComponent<TComponent>(m_container);
	}

	void destroy(Component* component);

private:
	std::shared_ptr<ComponentContainer> m_container;
	std::shared_ptr<ComponentRepository> m_componentRepository;
};

}
