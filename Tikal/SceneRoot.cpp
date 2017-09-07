#include "SceneRoot.h"

namespace tikal
{

SceneRoot::SceneRoot(std::shared_ptr<ComponentInstantiator> componentInstantiator) :
	m_componentInstantiator(componentInstantiator),
	m_pool(ObjectPool(sizeof(SceneObject), 1024 * 1024)) // TODO put size in config
{}

SceneRoot::~SceneRoot()
{
	// TODO delete all sceneObjects
}

SceneObject* SceneRoot::createSceneObject()
{
	auto location = m_pool.reserveLocation();
	auto sceneObject = new (location) SceneObject(this, m_componentInstantiator.get());
	return sceneObject;
}

void SceneRoot::destroySceneObject(SceneObject* sceneObject)
{
	sceneObject->~SceneObject();
	m_pool.freeLocation(sceneObject);
}

}
