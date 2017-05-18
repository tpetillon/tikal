#include "SceneRoot.h"

namespace tikal
{

using std::make_unique;

SceneRoot::SceneRoot(const ViewInstantiator & viewInstantiator) :
	m_viewInstantiator(viewInstantiator),
	m_rootObject(make_unique<SceneObject>(viewInstantiator))
{
}

}
