#include "Physics/PhysicsScene.hpp"
#include "Physics/PhysicsObject.hpp"
#include "Physics/Collider.hpp"
#include "Physics/Spring.hpp"
#include "Physics/AABBCollider.hpp"
#include "Physics/Tree.hpp"

#include <glm/geometric.hpp>
#include <imgui.h>
#include <chrono>

namespace Physics {

	Scene::Scene() {

		m_tree = new Tree();

	}

	Scene::~Scene() {

		//Clean up tree
		delete m_tree;

		//Clean up objects
		for(auto iter : m_Objects)
			delete iter;
		m_Objects.clear();

		//Clean up constraints
		for(auto iter : m_Constraints)
			delete iter;
		m_Constraints.clear();

	}

	void Scene::FixedUpdate() {

		m_CollisionPairs.clear();
		m_InCollisionLookup.clear();

		//Update Constraints
		for(auto iter : m_Constraints) {
			iter->FixedUpdate();
		}

		//Update Tree
		m_tree->Update(this);

		//Update all objects
		//for(auto iter : m_Objects) {
		//
		//	glm::vec3 currAccel = iter->GetAcceleration();
		//
		//	iter->SetAcceleration(currAccel + m_Gravity);
		//	iter->ApplyForce(m_GlobalForce);
		//	iter->FixedUpdate();
		//
		//	//Temp collision with ground
		//	auto& pos = iter->GetPosition();
		//	if(pos.y < 0.5f) {
		//		auto& vel = iter->GetVelocity();
		//		iter->SetPosition(glm::vec3(pos.x, 0.5f, pos.z));
		//		iter->SetVelocity(glm::vec3(vel.x, -vel.y, vel.z));
		//	}
		//
		//}

		m_GlobalForce = glm::vec3();

		//ImGui::Begin("Performance");
		//
		//std::chrono::steady_clock::time_point detectStart = std::chrono::steady_clock::now();
		//DetectCollisions();
		//std::chrono::steady_clock::time_point detectEnd = std::chrono::steady_clock::now();
		//std::chrono::duration<double> detectionLength = std::chrono::duration_cast<std::chrono::duration<double>>(detectEnd - detectStart);
		//
		//ImGui::Text("Detection time: %fms.", detectionLength.count() * 1000);
		//
		//std::chrono::steady_clock::time_point resolveStart = std::chrono::steady_clock::now();
		//ResolveCollisions();
		//std::chrono::steady_clock::time_point resolveEnd = std::chrono::steady_clock::now();
		//std::chrono::duration<double> resolveLength = std::chrono::duration_cast<std::chrono::duration<double>>(resolveEnd - resolveStart);
		//
		//ImGui::Text("Resolve time: %fms", resolveLength.count() * 1000);
		//
		//ImGui::End();

	}

	void Scene::AttachObject(Object * obj) {

		auto find = std::find(m_Objects.begin(), m_Objects.end(), obj);
		if(find != m_Objects.end())		return;
		
		m_Objects.push_back(obj);

		m_tree->Insert(obj);

	}

	void Scene::RemoveObject(Object * obj) {

		auto find = std::find(m_Objects.begin(), m_Objects.end(), obj);
		if(find == m_Objects.end())		return;

		delete (*find);
		m_Objects.erase(find);

	}

	void Scene::AttachConstraint(Constraint * con) {

		auto find = std::find(m_Constraints.begin(), m_Constraints.end(), con);
		if(find != m_Constraints.end())		return;

		m_Constraints.push_back(con);

	}

	void Scene::RemoveConstraint(Constraint * con) {

		auto find = std::find(m_Constraints.begin(), m_Constraints.end(), con);
		if(find == m_Constraints.end())	return;

		delete (*find);
		m_Constraints.erase(find);

	}

	//void Scene::DetectCollisions() {
	//
	//	m_CollisionPairs.clear();
	//	m_InCollisionLookup.clear();
	//
	//	//Brute force collision detection
	//	//TODO: Replace with OctTree
	//	for(auto iterA = m_Objects.begin(); iterA != m_Objects.end(); iterA++) {
	//		//Grab A's collider
	//		Collider* colliderA = (*iterA)->GetCollider();
	//		for(auto iterB = iterA + 1; iterB != m_Objects.end(); iterB++) {
	//			//Grab B's collider
	//			Collider* colliderB = (*iterB)->GetCollider();
	//			CollisionInfo info;
	//			//Check for intersection
	//			if(colliderA->Intersects(colliderB, &info.intersection)) {
	//				info.objA = (*iterA);
	//				info.objB = (*iterB);
	//
	//				m_CollisionPairs.push_back(info);
	//				m_InCollisionLookup[(*iterA)] = true;
	//				m_InCollisionLookup[(*iterB)] = true;
	//			}
	//		}
	//	}
	//
	//}
	//
	//void Scene::ResolveCollisions() {
	//
	//	//Loop through all collision pairs
	//	for(auto iter : m_CollisionPairs) {
	//
	//		//Get data from collision
	//
	//		//find out if objects are able to be moved
	//		const bool objAStatic = iter.objA->GetRigid();
	//		const bool objBStatic = iter.objB->GetRigid();
	//
	//		//If neither object can be moved then continue
	//		if(objAStatic && objBStatic)	continue;
	//
	//		//Collision normal (direction of collision and overlap)
	//		glm::vec3 colNorm = glm::normalize(iter.intersection.collisionVector);
	//
	//		//Mass of both objects
	//		float massA = iter.objA->GetMass();
	//		float massB = iter.objB->GetMass();
	//
	//		//Velocities of both objects (we might use relative velocity)
	//		glm::vec3 velA = iter.objA->GetVelocity();
	//		glm::vec3 velB = iter.objB->GetVelocity();
	//
	//		//Relative velocity
	//		glm::vec3 relVel = velA - velB;
	//
	//		//Find out how much velocity each object had in the collision normal direction
	//		//In fact, since we have the relative velocity, we can just find out once
	//		//how much total velocity there is in the collision normal direction
	//		glm::vec3 colVector = colNorm * (glm::dot(relVel, colNorm));
	//
	//		//Find the bounciness of the collision
	//		float bounciness = glm::min(iter.objA->GetBounciness(), iter.objB->GetBounciness());
	//
	//		//Calculate the impulse force (vector of force and direction)
	//		glm::vec3 impulse = (1.0f + bounciness) * colVector / (1.0f / massA + 1.0f / massB);
	//
	//		//Move the objects so that they're not overlapping
	//		glm::vec3 separate = iter.intersection.collisionVector * 0.5f;
	//
	//		if(objAStatic) {
	//
	//			//Calculate force to be reflected back to the non-static object
	//			glm::vec3 reflectForce = -1 * massB * colNorm * glm::dot(colNorm, velB);
	//
	//			iter.objB->SetVelocity(reflectForce);
	//
	//			iter.objB->SetPosition(iter.objB->GetPosition() + (separate * 2.0f));
	//
	//			continue;
	//		}
	//		if(objBStatic) {
	//
	//			//Calculate force to be reflected back to the non-static object
	//			glm::vec3 reflectForce = -1 * massA * colNorm * glm::dot(colNorm, velA);
	//
	//			iter.objA->SetVelocity(reflectForce);
	//
	//			iter.objA->SetPosition(iter.objA->GetPosition() - (separate * 2.0f));
	//			continue;
	//		}
	//
	//		//Apply that force to both objects (each one in an opposite direction)
	//		iter.objA->SetVelocity(velA - impulse * (1.0f / massA));
	//		iter.objB->SetVelocity(velB + impulse * (1.0f / massB));
	//
	//		iter.objB->SetPosition(iter.objB->GetPosition() + separate);
	//		iter.objA->SetPosition(iter.objA->GetPosition() - separate);			
	//
	//	}
	//
	//}
}