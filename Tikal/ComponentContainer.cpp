#include "ComponentContainer.h"

#include <stdexcept>
#include <string>

namespace tikal
{

using std::list;
using std::shared_ptr;

ComponentContainer::ComponentContainer(
	shared_ptr<Hypodermic::Container> container,
	list<shared_ptr<ComponentRegistration>> registrations) :
	m_container(container)
{
	for (auto& registration : registrations)
	{
		m_registrations[registration->instanceType()] = registration;
	}
}

void* ComponentContainer::instantiateComponent(
	Hypodermic::TypeInfo type, void* const placement, SceneObject* sceneObject)
{
	auto it = m_registrations.find(type);

	if (it == m_registrations.end())
	{
		using namespace std::string_literals;
		throw std::invalid_argument("Component type not registered"s);
	}

	return (it->second->factory())(*(m_container.get()), placement, sceneObject);
}

}
