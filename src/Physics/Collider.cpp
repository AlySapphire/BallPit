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
					return Sphere2AABB((SphereCollider*)this, (AABBCollider*)other, intersection);
			}
		} else if(m_Type == ColliderType::AABB) {
			switch(other->GetType()) {
				case ColliderType::SPHERE:
					return AABB2Sphere((AABBCollider*)this, (SphereCollider*)other, intersection);
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
		intersection->intersectionType = CollisionType::SPHERE2SPHERE;

		return (dist < minDist);

	}

	bool Collider::Sphere2AABB(SphereCollider * objA, AABBCollider * objB, IntersectData * intersection) {

		//Cache the centre and extents of the AABB and sphere centre
		auto& boxCentre = objB->GetCentre();
		auto& extents = objB->GetExtents();
		auto& sphereCentre = objA->GetPosition();
		
		//Get the min and max of the box
		glm::vec3 boxMin = boxCentre - extents;
		glm::vec3 boxMax = boxCentre + extents;
		
		//Get closest point to sphere centre by clamping
		glm::vec3 closestPoint = glm::max(boxMin, glm::min(sphereCentre, boxMax));
		
		//Calculate a direction vector
		glm::vec3 dirVec = closestPoint - sphereCentre;
		
		//Get the length of the vector
		float dist = glm::length(dirVec);
		//This one is incorrect. It needs to be the normalized direction vector and the overlap length
		intersection->collisionVector = glm::normalize(dirVec);
		intersection->intersectionType = CollisionType::SPHERE2AABB;
		
		//Compare it to the sphere radius
		return (dist < objA->GetRadius());
		
	}

	bool Collider::AABB2Sphere(AABBCollider* objA, SphereCollider* objB, IntersectData* intersection) {
		//Cache the centre and extents of the AABB and sphere centre
		auto& boxCentre = objA->GetCentre();
		auto& extents = objA->GetExtents();
		auto& sphereCentre = objB->GetPosition();

		//Get the min and max of the box
		glm::vec3 boxMin = boxCentre - extents;
		glm::vec3 boxMax = boxCentre + extents;

		//Get closest point to sphere centre by clamping
		glm::vec3 closestPoint = glm::max(boxMin, glm::min(sphereCentre, boxMax));

		//Calculate a direction vector
		glm::vec3 dirVec = closestPoint - sphereCentre;

		//Get the length of the vector
		float dist = glm::length(dirVec);
		//This one is incorrect. It needs to be the normalized direction vector and the overlap length
		intersection->collisionVector = glm::normalize(dirVec);
		intersection->intersectionType = CollisionType::AABB2SPHERE;

		//Compare it to the sphere radius
		return (dist < objB->GetRadius());
	}

}


