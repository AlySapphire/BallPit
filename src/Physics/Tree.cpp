#include "Physics/Tree.hpp"
#include "Physics/PhysicsObject.hpp"
#include "Physics/AABBCollider.hpp"
#include "Physics/SphereCollider.hpp"
#include "Physics/PhysicsScene.hpp"

#include <glm/geometric.hpp>
#include <imgui.h>
#include <chrono>

namespace Physics {

	bool Tree::m_treebuilt = false;

	Tree::Tree() : m_parent(nullptr) {
	}

	Tree::Tree(Tree * parent) : m_parent(parent) {
	}

	Tree::Tree(const std::vector<Object*>& objects) : m_parent(nullptr), m_objects(objects) {
	}

	Tree::Tree(Tree * parent, const std::vector<Object*>& objects, const glm::vec3& regionDir) : m_parent(parent), m_objects(objects), m_regionDir(regionDir) {

	}

	Tree::~Tree() {

		//Clean up child nodes
		for(auto iter : m_childNodes)
			delete iter;

	}

	bool Tree::Insert(Object * obj) {

		//Check the object does currently exist
		auto find = std::find(m_objects.begin(), m_objects.end(), obj);

		if(find != m_objects.end())		return false;

		//Go through child nodes and try to add the object
		for(auto iter : m_childNodes) {
			if(iter->Insert(obj))		return true;
		}

		//Check if we are a leaf
		if(m_parent != nullptr) { 
			if(fit(obj, m_regionDir)) {
				m_objects.push_back(obj);
				return true;
			} else {
				return false;
			}
		} 
		//If we are the root and no object is able to insert the object then keep it on the parent list
		else {
			m_objects.push_back(obj);
		}

		return true;

	}

	void Tree::BuildTree() {

		//If we are the parent then build the child nodes
		if(m_parent == nullptr) {

			int directionsX[] = { 1, 1, -1, -1 };
			int directionsZ[] = { 1, -1, 1, -1 };

			for(int i = 0; i < 4; i++) {

				glm::vec3 regionDir = glm::vec3(directionsX[i], 0, directionsZ[i]);

				//List of contained objects
				std::vector<Object*> containedObjects;

				//Check collider types on the objects and cast to check bounds
				for(auto iter : m_objects) {

					if(fit(iter, regionDir))
						containedObjects.push_back(iter);
				}

				//Erase the objects we have moved into appropriate regions
				for(auto iter = containedObjects.begin(); iter != containedObjects.end(); iter++) {
					auto seeker = std::find(m_objects.begin(), m_objects.end(), (*iter));
					m_objects.erase(seeker);
				}			

				//Create a new tree
				Tree* child = new Tree(this, containedObjects, regionDir);

				m_childNodes.push_back(child);

			}
		}
		m_treebuilt = true;
	}

	void Tree::Update(Scene* scene) {

		if(!m_treebuilt)
			BuildTree();

		std::vector<Object*> movedObjects;

		//Update objects
		for(auto obj : m_objects) {

			//Apply scene gravity
			glm::vec3 currAccel = obj->GetAcceleration();
			obj->SetAcceleration(currAccel + scene->GetGravity());
			obj->ApplyForce(scene->m_GlobalForce);

			if(obj->FixedUpdate())
				movedObjects.push_back(obj);

			//Temp collision with ground
			auto& pos = obj->GetPosition();

			if(pos.y < 0.5f) {
				auto& vel = obj->GetVelocity();
				obj->SetPosition(glm::vec3(pos.x, 0.5f, pos.z));
				obj->SetVelocity(glm::vec3(vel.x, -vel.y, vel.z));
			}

		}


		//Update children
		for(auto child : m_childNodes)
			child->Update(scene);

		//Place objects where they need to be
		for(auto obj = movedObjects.begin(); obj != movedObjects.end(); obj++) {
			
			Tree* current = this;

			//Check if we fit
			while(current->m_parent != nullptr) {
				if(!fit((*obj), m_regionDir))		current = current->m_parent;
				else break;
			}
			auto seeker = std::find(m_objects.begin(), m_objects.end(), (*obj));

			m_objects.erase(seeker);
			current->Insert((*obj));

		}

		//After Objects are where they need to be we can detect collisions
		if(m_parent == nullptr) {
			ImGui::Begin("Performance");
			std::chrono::steady_clock::time_point detectStart = std::chrono::steady_clock::now();
			DetectCollisions(scene);
			std::chrono::steady_clock::time_point detectEnd = std::chrono::steady_clock::now();
			std::chrono::duration<double> detectionLength = std::chrono::duration_cast<std::chrono::duration<double>>(detectEnd - detectStart);
			
			ImGui::Text("Detection time: %fms.", detectionLength.count() * 1000);

			std::chrono::steady_clock::time_point resolveStart = std::chrono::steady_clock::now();
			ResolveCollisions(scene);
			std::chrono::steady_clock::time_point resolveEnd = std::chrono::steady_clock::now();
			std::chrono::duration<double> resolveLength = std::chrono::duration_cast<std::chrono::duration<double>>(resolveEnd - resolveStart);
			
			ImGui::Text("Resolve time: %fms", resolveLength.count() * 1000);
			ImGui::End();
		}

	}

	void Tree::DetectCollisions(Scene * scene, std::vector<Object*>* parentObjs) {

		//Check parent objects against one another
		if(parentObjs != nullptr) {
			for(auto iterA = parentObjs->begin(); iterA != parentObjs->end(); iterA++) {
				//Grab A's Collider
				Collider* colliderA = (*iterA)->GetCollider();
				for(auto iterB = m_objects.begin(); iterB != m_objects.end(); iterB++) {
					//Grab B's Collider
					Collider* colliderB = (*iterB)->GetCollider();
					Scene::CollisionInfo info;
					//Check for intersection
					if(colliderA->Intersects(colliderB, &info.intersection)) {
						info.objA = (*iterA);
						info.objB = (*iterB);

						scene->m_CollisionPairs.push_back(info);
						scene->m_InCollisionLookup[(*iterA)] = true;
						scene->m_InCollisionLookup[(*iterB)] = true;
					}
				}
			}
		}

		//Now check local objects against one another
		for(auto iterA = m_objects.begin(); iterA != m_objects.end(); iterA++) {
			//Grab A's Collider
			Collider* colliderA = (*iterA)->GetCollider();
			for(auto iterB = iterA + 1; iterB != m_objects.end(); iterB++) {
				//Grab B's Collider
				Collider* colliderB = (*iterB)->GetCollider();
				Scene::CollisionInfo info;
				//Check for intersection
				if(colliderA->Intersects(colliderB, &info.intersection)) {
					info.objA = (*iterA);
					info.objB = (*iterB);

					scene->m_CollisionPairs.push_back(info);
					scene->m_InCollisionLookup[(*iterA)] = true;
					scene->m_InCollisionLookup[(*iterB)] = true;
				}
			}
		}

		for(auto child : m_childNodes) {
			if(parentObjs != nullptr) {

				m_objects.insert(m_objects.end(), parentObjs->begin(), parentObjs->end());
			}
			child->DetectCollisions(scene, &m_objects);
		}
			
	}

	void Tree::ResolveCollisions(Scene * scene) {
		//Loop through all collision pairs
		for(auto iter : scene->m_CollisionPairs) {
		
			//Get data from collision
		
			//find out if objects are able to be moved
			const bool objAStatic = iter.objA->GetRigid();
			const bool objBStatic = iter.objB->GetRigid();
		
			//If neither object can be moved then continue
			if(objAStatic && objBStatic)	continue;
		
			//Collision normal (direction of collision and overlap)
			glm::vec3 colNorm = glm::normalize(iter.intersection.collisionVector);
		
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
		
			//Move the objects so that they're not overlapping
			glm::vec3 separate = iter.intersection.collisionVector * 0.5f;
		
			if(objAStatic) {
		
				//Calculate force to be reflected back to the non-static object
				glm::vec3 reflectForce = -1 * massB * colNorm * glm::dot(colNorm, velB);
		
				iter.objB->SetVelocity(reflectForce);
		
				iter.objB->SetPosition(iter.objB->GetPosition() + (separate * 2.0f));
		
				continue;
			}
			if(objBStatic) {
		
				//Calculate force to be reflected back to the non-static object
				glm::vec3 reflectForce = -1 * massA * colNorm * glm::dot(colNorm, velA);
		
				iter.objA->SetVelocity(reflectForce);
		
				iter.objA->SetPosition(iter.objA->GetPosition() - (separate * 2.0f));
				continue;
			}
		
			//Apply that force to both objects (each one in an opposite direction)
			iter.objA->SetVelocity(velA - impulse * (1.0f / massA));
			iter.objB->SetVelocity(velB + impulse * (1.0f / massB));
		
			iter.objB->SetPosition(iter.objB->GetPosition() + separate);
			iter.objA->SetPosition(iter.objA->GetPosition() - separate);			
		
		}
	}

	bool Tree::fit(Object * obj, const glm::vec3& dir) {

		switch(obj->GetCollider()->GetType()) {
			case Collider::ColliderType::SPHERE: {
				//Cast to sphere collider
				SphereCollider* sc = (SphereCollider*)obj->GetCollider();

				//Cache values we need to check
				auto& pos = obj->GetPosition();
				float radius = sc->GetRadius();

				//Check if we fit in the bounds
				if((dir.x > 0) ? pos.x - radius >= dir.x : pos.x + radius <= dir.x)
					if((dir.z > 0) ? pos.z - radius >= dir.z : pos.z + radius <= dir.z)
						return true;
				
			}
				break;
			case Collider::ColliderType::AABB: {
				//Cast to AABB collider
				AABBCollider* ac = (AABBCollider*)obj->GetCollider();

				//Cache values we need to check
				auto& boxCentre = ac->GetCentre();
				auto& boxExtents = ac->GetExtents();

				//Calculate min and max
				glm::vec3 boxMin = boxCentre - boxExtents;
				glm::vec3 boxMax = boxCentre + boxExtents;

				if((dir.x > 0) ? boxMin.x >= dir.x : boxMax.x <= dir.x)
					if((dir.z > 0) ? boxMin.z >= dir.z : boxMax.z <= dir.z)
						return true;
			}
				
				break;
		}

		return false;
	}

}