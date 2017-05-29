#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <vector>

namespace Physics {
	class Object;
	class Scene;
	class GizmosRenderer;
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
	
	Physics::Scene* m_PhysicsScene;
	Physics::GizmosRenderer* m_GizmosRenderer;

	void DrawGrid();

};