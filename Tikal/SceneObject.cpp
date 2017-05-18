#include "SceneObject.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include "View.h"

namespace tikal
{

using std::unique_ptr;

SceneObject::SceneObject(const ViewInstantiator& viewInstantiator) :
	m_viewInstantiator(viewInstantiator),
	m_parent(nullptr),
	m_active(true)
{
}

SceneObject::~SceneObject()
{
}

void SceneObject::addChild(unique_ptr<SceneObject> newChild)
{
	using namespace std::string_literals;

	if (newChild.get() == this)
	{
		throw std::invalid_argument("Cannot set an object to be its own child"s);
	}

	auto object = this;
	do
	{
		if (object == newChild.get())
		{
			throw std::invalid_argument("Cannot add parent object as child as it would create a loop"s);
		}
		object = object->parent();
	} while (object != nullptr);

	m_children.push_back(std::move(newChild));

	newChild->m_parent = this;
}

unique_ptr<SceneObject> SceneObject::removeChild(SceneObject* child)
{
	using namespace std::string_literals;

	const auto it = std::find_if(
		m_children.begin(), m_children.end(),
		[child](unique_ptr<SceneObject>& c) { return c.get() == child; });

	if (it == m_children.end())
	{
		throw std::invalid_argument("Given parameter is not a child"s);
	}

	auto foundChild = std::move(*it);

	m_children.erase(it);

	return std::move(foundChild);
}

}
