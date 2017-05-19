#pragma once

namespace tikal
{

template<typename TBaseView>
class SceneObject;

template<typename TBaseView>
class View
{
	friend SceneObject<TBaseView>;

public:
	View() {}

	View(View const&) = delete;
	void operator=(View const&) = delete;

	SceneObject<TBaseView>* sceneObject() const { return m_sceneObject; }

private:
	SceneObject<TBaseView>* m_sceneObject;
};

}
