#include "SceneObject.h"

namespace tikal
{

using std::shared_ptr;

SceneObject::SceneObject(SceneRoot* sceneRoot, ComponentInstantiator* componentInstantiator) :
	m_sceneRoot(sceneRoot), m_componentInstantiator(componentInstantiator), m_name("")
{}

SceneObject::SceneObject(SceneRoot* sceneRoot, ComponentInstantiator* componentInstantiator, std::string name) :
	m_sceneRoot(sceneRoot), m_componentInstantiator(componentInstantiator), m_name(name)
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
