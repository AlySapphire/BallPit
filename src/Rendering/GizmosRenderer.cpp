#include "Rendering/GizmosRenderer.hpp"

#include "Physics/PhysicsObject.hpp"
#include "Physics/PhysicsScene.hpp"
#include "Physics/SphereCollider.hpp"
#include "Physics/Spring.hpp"

#include <Gizmos.h>

namespace Physics {

	GizmosRenderer::GizmosRenderer() {
	}

	GizmosRenderer::~GizmosRenderer() {
	}

	void GizmosRenderer::Draw(Scene * scene) {

		RenderGizmosObjects(scene);

	}

	void GizmosRenderer::RenderGizmosObjects(Scene * scene) {

		auto& objects = scene->GetObjects();

		for(auto iter : objects) {

			//Get the current objects' collider and find out whether it's in a collision
			Collider* collider = iter->GetCollider();
			bool isInCollision = scene->IsInCollision(iter);

			//Get color information
			glm::vec4 color = (isInCollision) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : GetRenderInfo(iter)->color;

			//Render Sphere Collider
			if(collider->GetType() == Collider::ColliderType::SPHERE) {
				//Cast to Sphere Collider and draw with Gizmos
				SphereCollider* sc = (SphereCollider*)collider;
				aie::Gizmos::addSphere(sc->GetPosition(), sc->GetRadius(), 8, 8, color);
			} 
			//Render AABB
			else if(collider->GetType() == Collider::ColliderType::AABB) {
				//TODO: Render AABB
			}

		}

	}

	void GizmosRenderer::RenderGizmosConstraints(Scene * scene) {

		for(auto& iter : scene->GetConstraints()) {

			switch(iter->GetType()) {
				case Constraint::ConstraintType::SPRING:
					Object* objA = nullptr;
					Object* objB = nullptr;
					iter->GetConnections(objA, objB);
					aie::Gizmos::addLine(objA->GetPosition(), objB->GetPosition(), GetRenderInfo((Spring*)iter)->color);
					break;
				case Constraint::ConstraintType::JOINT:
					//TODO: implement joint drawing
					break;
			}

		}

	}

}