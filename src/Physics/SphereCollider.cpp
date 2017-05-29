#include "Physics/SphereCollider.hpp"
#include "Physics/PhysicsObject.hpp"

namespace Physics {

	SphereCollider::SphereCollider() : Collider(ColliderType::SPHERE), m_Radius(1.0f) {
	}

	SphereCollider::SphereCollider(float radius) : Collider(ColliderType::SPHERE), m_Radius(radius) {
	}

	SphereCollider::~SphereCollider() {
	}

	void SphereCollider::Transform(Object * obj) {
		m_Position = obj->GetPosition();
	}

}