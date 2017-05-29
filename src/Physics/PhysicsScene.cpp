#include "Physics/PhysicsScene.hpp"
#include "Physics/PhysicsObject.hpp"
#include "Physics/Collider.hpp"

#include <glm/geometric.hpp>

namespace Physics {

	Scene::Scene() {
	}

	Scene::~Scene() {

		//Clean up objects
		for(auto iter : m_Objects)
			delete iter;
		m_Objects.clear();

	}

	void Scene::FixedUpdate() {

		//Update all objects
		for(auto iter : m_Objects) {

			glm::vec3 currAccel = iter->GetAcceleration();

			iter->SetAcceleration(currAccel + m_Gravity);
			iter->ApplyForce(m_GlobalForce);
			iter->FixedUpdate();

			//Temp collision with ground
			auto& pos = iter->GetPosition();
			if(pos.y < 0.5f) {
				auto& vel = iter->GetVelocity();
				iter->SetPosition(glm::vec3(pos.x, 0.5f, pos.z));
				iter->SetVelocity(glm::vec3(vel.x, -vel.y, vel.z));
			}

		}

		m_GlobalForce = glm::vec3();

		DetectCollisions();
		ResolveCollisions();

	}

	void Scene::AttatchObject(Object * obj) {

		auto find = std::find(m_Objects.begin(), m_Objects.end(), obj);
		if(find != m_Objects.end())		return;

		m_Objects.push_back(obj);

	}

	void Scene::RemoveObject(Object * obj) {

		auto find = std::find(m_Objects.begin(), m_Objects.end(), obj);
		if(find == m_Objects.end())		return;

		delete (*find);
		m_Objects.erase(find);

	}

	void Scene::DetectCollisions() {

		m_CollisionPairs.clear();
		m_InCollisionLookup.clear();

		//Brute force collision detection
		//TODO: Replace with OctTree
		for(auto iterA = m_Objects.begin(); iterA != m_Objects.end(); iterA++) {
			//Grab A's collider
			Collider* colliderA = (*iterA)->GetCollider();
			for(auto iterB = iterA + 1; iterB != m_Objects.end(); iterB++) {
				//Grab B's collider
				Collider* colliderB = (*iterB)->GetCollider();
				CollisionInfo info;
				//Check for intersection
				if(colliderA->Intersects(colliderB, &info.intersection)) {
					info.objA = (*iterA);
					info.objB = (*iterB);

					m_CollisionPairs.push_back(info);
					m_InCollisionLookup[(*iterA)] = true;
					m_InCollisionLookup[(*iterB)] = true;
				}
			}
		}

	}

	void Scene::ResolveCollisions() {

		//Loop through all collision pairs
		for(auto iter : m_CollisionPairs) {

			//Get data from collision

			//Collision normal (directon of collision and overlap)
			glm::vec3 colNorm = iter.intersection.collisionVector;

			//Mass of both objects
			float massA = iter.objA->GetMass();
			float massB = iter.objB->GetMass();

			//Velocities of both objects (we might use relative velocity)
			glm::vec3 velA = iter.objA->GetVelocity();
			glm::vec3 velB = iter.objB->GetVelocity();

			//Relative velocity
			glm::vec3 relVel = velA - velB;

			//Find out how much velocity each object had in the collision normal direction
			//In fact, since we have the relative velocity, we can just find out once
			//how much total velocity there is in the collision normal direction
			glm::vec3 colVector = colNorm * (glm::dot(relVel, colNorm));

			//Find the bounciness of the collision
			float bounciness = glm::min(iter.objA->GetBounciness(), iter.objB->GetBounciness());

			//Calculate the impulse force (vector of force and direction)
			glm::vec3 impulse = (1.0f + bounciness) * colVector / (1.0f / massA + 1.0f / massB);

			//Apply that force to both objects (each one in an opposite direction)
			iter.objA->SetVelocity(velA - impulse * (1.0f / massA));
			iter.objB->SetVelocity(velB + impulse * (1.0f / massB));

			//Move the spheres so that they're not overlapping
			glm::vec3 separate = iter.intersection.collisionVector * 0.5f;
			iter.objA->SetPosition(iter.objA->GetPosition() - separate);
			iter.objB->SetPosition(iter.objB->GetPosition() + separate);

		}

	}
}