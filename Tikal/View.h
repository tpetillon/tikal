#pragma once

//#include <iostream>

namespace tikal
{

template<typename TBaseView>
class SceneObject;

template<typename TBaseView>
class View
{
	friend SceneObject<TBaseView>;

public:
	View()
	{
		//std::cout << "View constructed" << std::endl;
	}

	virtual ~View()
	{
		//std::cout << "View destructed" << std::endl;
	}

	View(View const&) = delete;
	void operator=(View const&) = delete;

	SceneObject<TBaseView>* sceneObject() const { return m_sceneObject; }

private:
	SceneObject<TBaseView>* m_sceneObject;
};

}
