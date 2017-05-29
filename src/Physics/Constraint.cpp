#include "Physics/Constraint.hpp"
#include "Physics/PhysicsObject.hpp"

namespace Physics {

	Constraint::Constraint( Object * objA, Object * objB, ConstraintType type) : m_ObjA(objA), m_ObjB(objB), m_Type(type) {
	}

	Constraint::~Constraint() {
	}

}