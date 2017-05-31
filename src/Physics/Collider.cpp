#include "Physics/Collider.hpp"
#include "Physics/SphereCollider.hpp"
#include "Physics/AABBCollider.hpp"

#include <glm/geometric.hpp>

namespace Physics {

	Collider::Collider(ColliderType type) : m_Type(type) {
	}


	Collider::~Collider() {
	}

	bool Collider::Intersects(Collider * other, IntersectData * intersection) {
		
		if(m_Type == ColliderType::SPHERE) {
			switch(other->GetType()) {
				case ColliderType::SPHERE:
					return Sphere2Sphere((SphereCollider*)this, (SphereCollider*)other, intersection);
				case ColliderType::AABB:
					//TODO: Implement Sphere2AABB
					break;
			}
		} else if(m_Type == ColliderType::AABB) {
			switch(other->GetType()) {
				case ColliderType::SPHERE:
					//TODO: Implement AABB2Sphere
					break;
				case ColliderType::AABB:
					//TODO: Implement AABB2AABB
					break;
			}
		}
		
		return false;
	}

	bool Collider::Sphere2Sphere(SphereCollider * objA, SphereCollider * objB, IntersectData * intersection) {
		//Create direction vector
		glm::vec3 dirVec = objB->GetPosition() - objA->GetPosition();

		//Calculate distance and total of both radii so that we can calculate if if we've intersected
		float dist = glm::length(dirVec);
		float minDist = objA->GetRadius() + objB->GetRadius();

		//Create collision vector which is the normalized direction vector and the length of the overlap
		dirVec = glm::normalize(dirVec) * (minDist - dist);
		intersection->collisionVector = dirVec;

		return (dist < minDist);

	}

	bool Collider::Sphere2AABB(SphereCollider * objA, AABBCollider * objB, IntersectData * intersection) {
		//TODO: Implement Sphere to AABB intersection logic
		
		return false;
	}

}


