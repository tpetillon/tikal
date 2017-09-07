#include "ComponentRepository.h"

#include <algorithm>

#include "Component.h"

namespace tikal
{

using Hypodermic::TypeInfo;

using std::shared_ptr;

ComponentRepository::ComponentRepository()
{}

std::unique_ptr<ObjectPool>& ComponentRepository::getOrCreatePool(
	Hypodermic::TypeInfo type, size_t objectSize, size_t blockSize)
{
	auto it = m_componentPools.find(type);

	if (it != m_componentPools.end())
	{
		return it->second;
	}

	m_componentPools[type] = std::make_unique<ObjectPool>(objectSize, blockSize);

	return m_componentPools.at(type);
}

void ComponentRepository::destroyComponent(Component* component)
{
	auto type = component->type();

	component->~Component();

	m_componentPools[type]->freeLocation(static_cast<void*>(component));
}

}
