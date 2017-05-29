#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

class Camera;

class BallPitApp : public aie::Application {
public:

	BallPitApp();
	virtual ~BallPitApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Camera* m_Camera;

	void DrawGrid();

};