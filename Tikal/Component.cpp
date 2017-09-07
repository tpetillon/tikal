#include "Component.h"

#include <stdexcept>
#include <string>

namespace tikal
{

using std::runtime_error;

void Component::attach(SceneObject* const sceneObject)
{
	if (m_sceneObject != nullptr)
	{
		using namespace std::string_literals;

		throw runtime_error("Component is already attached to a scene object"s);
	}

	m_sceneObject = sceneObject;
}

};
