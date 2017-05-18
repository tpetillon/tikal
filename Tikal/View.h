#pragma once

namespace tikal
{

class SceneObject;

class View
{
public:
	View(View const&) = delete;
	void operator=(View const&) = delete;

	virtual void addDrawCommands(/*FrameState frameState*/) = 0;
	virtual void drawGui() = 0;

protected:
	SceneObject& m_sceneObject;
};

}
