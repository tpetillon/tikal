#pragma once

#include <memory>

#include "SceneObject.h"
#include "ViewInstantiator.h"

namespace tikal
{

class SceneRoot
{
public:
	SceneRoot(const ViewInstantiator& ViewInstantiator);

private:
	const ViewInstantiator& m_viewInstantiator;

	std::unique_ptr<SceneObject> m_rootObject;
};

}
