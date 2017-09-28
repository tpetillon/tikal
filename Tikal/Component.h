#pragma once

#include <Hypodermic/TypeInfo.h>

#include "Destroyable.h"

namespace tikal
{

class SceneObject;

class Component : public Destroyable
{
public:
	Component(SceneObject* sceneObject) : m_sceneObject(sceneObject) {}

	virtual Hypodermic::TypeInfo type() const = 0;

	SceneObject* sceneObject() const { return m_sceneObject; }

private:
	SceneObject* const m_sceneObject;
};

}
