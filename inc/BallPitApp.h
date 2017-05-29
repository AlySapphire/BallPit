#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>

namespace Physics {
	class Object;
}

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
	
	std::vector<Physics::Object*> m_Objects;

	void DrawGrid();

};