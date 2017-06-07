#include "Physics/Collider.hpp"
#include "Physics/SphereCollider.hpp"
#include "Physics/AABBCollider.hpp"

#include <glm/geometric.hpp>
#include <iostream>

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
		float sphereRadius = objA->GetRadius();
		
		//Get the min and max of the box
		glm::vec3 boxMin = boxCentre - extents;
		glm::vec3 boxMax = boxCentre + extents;
		
		//Get closest point to sphere centre by clamping
		float x = glm::max(boxMin.x, glm::min(sphereCentre.x, boxMax.x));
		float y = glm::max(boxMin.y, glm::min(sphereCentre.y, boxMax.y));
		float z = glm::max(boxMin.z, glm::min(sphereCentre.z, boxMax.z));

		float dist = glm::sqrt(glm::pow(x - sphereCentre.x, 2) + 
							   glm::pow(y - sphereCentre.y, 2) +
							   glm::pow(z - sphereCentre.z,2));
		
		//Store collision data
		if(intersection != nullptr) {
			glm::vec3 collisionNormal = glm::normalize(boxCentre - sphereCentre);
			float overlap = sphereRadius - dist;
			intersection->collisionVector = collisionNormal * overlap;
			intersection->intersectionType = CollisionType::SPHERE2AABB;
		}		
		
		//Compare it to the sphere radius
		return (dist < sphereRadius);
		
	}

	bool Collider::AABB2Sphere(AABBCollider* objA, SphereCollider* objB, IntersectData* intersection) {
		//Cache the centre and extents of the AABB and sphere centre
		auto& boxCentre = objA->GetCentre();
		auto& extents = objA->GetExtents();
		auto& sphereCentre = objB->GetPosition();
		float sphereRadius = objB->GetRadius();

		//Get the min and max of the box
		glm::vec3 boxMin = boxCentre - extents;
		glm::vec3 boxMax = boxCentre + extents;

		//Get closest point to sphere centre by clamping
		float x = glm::max(boxMin.x, glm::min(sphereCentre.x, boxMax.x));
		float y = glm::max(boxMin.y, glm::min(sphereCentre.y, boxMax.y));
		float z = glm::max(boxMin.z, glm::min(sphereCentre.z, boxMax.z));

		float dist = glm::sqrt(glm::pow(x - sphereCentre.x, 2) +
			glm::pow(y - sphereCentre.y, 2) +
			glm::pow(z - sphereCentre.z, 2));

		if(intersection != nullptr) {
			glm::vec3 collisionNormal = glm::normalize(sphereCentre - boxCentre);
			float overlap = sphereRadius - dist;
			intersection->collisionVector = collisionNormal * overlap;
			intersection->intersectionType = CollisionType::SPHERE2AABB;
		}
		

		//Compare it to the sphere radius
		return (dist < sphereRadius);
	}

}


