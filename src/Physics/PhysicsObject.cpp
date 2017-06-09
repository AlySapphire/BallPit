#include "Physics/PhysicsObject.hpp"
#include "Physics/Collider.hpp"

#include <glm/geometric.hpp>

namespace Physics {

	Object::Object() : m_Mass(1.0f), m_Friction(1.0f), m_Bounciness(1.0f), m_Collider(nullptr), m_MaxVelocity(glm::vec3(20, 30, 20)) {
	}

	Object::~Object() {
		delete m_Collider;
	}

	bool Object::FixedUpdate() {

		glm::vec3 posOld = m_Position;

		float fixedTime = 1.0f / 60.0f;

		ApplyForce(-m_Velocity * m_Friction);

		m_Velocity += m_Acceleration * fixedTime;		
		m_Position += m_Velocity * fixedTime;
		m_Acceleration = glm::vec3();
		UpdateTransform();

		glm::vec3 movementDiff = glm::abs(m_Position - posOld);

		return(movementDiff.x > 0.1f || movementDiff.y > 0.1f || movementDiff.z > 0.1f);

	}

	void Object::ApplyForce(const glm::vec3 & a_Force) {

		m_Acceleration += a_Force / m_Mass;

	}

	Collider * Object::GetCollider() {
		if(m_Collider == nullptr) {
			static Collider nullCollider(Collider::ColliderType::NONE);
			return &nullCollider;
		}
		return m_Collider;
	}

	void Object::SetPosition(const glm::vec3 & a_Pos) {
		m_Position = a_Pos;
		UpdateTransform();
	}

	void Object::SetCollider(Collider * coll) {
		delete m_Collider;
		m_Collider = coll;
	}

	void Object::UpdateTransform() {
		if(m_Collider != nullptr)
			m_Collider->Transform(this);
	}

}