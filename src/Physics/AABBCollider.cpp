#include "Physics/AABBCollider.hpp"
#include "Physics/PhysicsObject.hpp"

namespace Physics {

	AABBCollider::AABBCollider() : Collider(ColliderType::AABB), m_Centre(glm::vec3(0)), m_Extents(glm::vec3(1)) {
	}

	AABBCollider::AABBCollider(const glm::vec3 & centre, const glm::vec3 & extents) : Collider(ColliderType::AABB),
	 m_Centre(centre), m_Extents(extents) {

	}


	AABBCollider::~AABBCollider() {
	}

	void AABBCollider::Transform(Object * obj) {

		m_Centre = obj->GetPosition();

	}

}


