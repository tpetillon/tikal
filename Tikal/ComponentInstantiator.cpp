#include "ComponentInstantiator.h"

#include <Hypodermic/Container.h>

namespace tikal
{

using std::shared_ptr;

ComponentInstantiator::ComponentInstantiator(
	shared_ptr<ComponentContainer> container, shared_ptr<ComponentRepository> componentRepository) :
	m_container(container), m_componentRepository(componentRepository)
{}

void ComponentInstantiator::destroy(Component* component)
{
	m_componentRepository->destroyComponent(component);
}

}
