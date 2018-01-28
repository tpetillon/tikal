#include "SceneObject.h"

namespace tikal
{

using std::shared_ptr;

SceneObject::SceneObject(Scene* scene, ComponentInstantiator* componentInstantiator) :
	m_scene(scene), m_componentInstantiator(componentInstantiator), m_name("")
{}

SceneObject::SceneObject(Scene* scene, ComponentInstantiator* componentInstantiator, std::string name) :
	m_scene(scene), m_componentInstantiator(componentInstantiator), m_name(name)
{
}

SceneObject::~SceneObject()
{
	for (auto& pair : m_components)
	{
		m_componentInstantiator->destroy(pair.second);
	}
}

}
