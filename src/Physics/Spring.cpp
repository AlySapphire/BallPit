#include "Physics/Spring.hpp"
#include "Physics/PhysicsObject.hpp"

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

namespace Physics {

	Spring::Spring(Object * objA, Object * objB) : Constraint(objA, objB, ConstraintType::SPRING),
	m_Length(5.0f), m_Stiffness(100.0f), m_Friction(1.0f) {

	}

	Spring::Spring(Object * objA, Object * objB, float length, float stiffness, float friction) : 
	Constraint(objA, objB, ConstraintType::SPRING), m_Length(length), m_Stiffness(stiffness), m_Friction(friction) {
	
	}

	Spring::~Spring() {

	}

	void Spring::FixedUpdate() {

		//Measure the distance between the 2 objects
		glm::vec3 springVec = m_ObjB->GetPosition() - m_ObjA->GetPosition();
		float dist = glm::length(springVec);

		//If we're at our length then no force needs to be applied
		if(dist == m_Length)	return;

		//Create a force along the vector between them based on how far away we are from our length
		glm::vec3 force = glm::normalize(springVec) * (dist - m_Length) * m_Stiffness;
		//Edit that force for friction (the faster things are moving, the more we slow them down)
		force += (glm::dot(m_ObjB->GetVelocity() - m_ObjA->GetVelocity(), springVec)) * glm::normalize(springVec) * m_Friction;

		//Apply the force to both objects in the required direction
		m_ObjA->ApplyForce(force);
		m_ObjB->ApplyForce(-force);

	}

}