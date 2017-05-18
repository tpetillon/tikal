#pragma once

#include <memory>

namespace tikal
{

class ViewInstantiator
{
public:
	template<typename TView>
	std::shared_ptr<TView> instantiate();
};

}
