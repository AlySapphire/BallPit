#include "Physics/PhysicsObject.hpp"

namespace Physics {

	Object::Object() : m_Mass(1.0f), m_Friction(1.0f), m_Bounciness(1.0f) {
	}

	Object::~Object() {
	}

	void Object::FixedUpdate() {

		float fixedTime = 1.0f / 60.0f;

		ApplyForce(-m_Velocity * m_Friction);

		m_Velocity += m_Acceleration * fixedTime;
		m_Position += m_Velocity * fixedTime;
		m_Acceleration = glm::vec3();

	}

	void Object::ApplyForce(const glm::vec3 & a_Force) {

		m_Acceleration += a_Force / m_Mass;

	}

	void Object::SetPosition(const glm::vec3 & a_Pos) {
		m_Position = a_Pos;
	}

}