#include "Physics/PhysicsScene.hpp"
#include "Physics/PhysicsObject.hpp"
#include "Physics/Collider.hpp"

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

}