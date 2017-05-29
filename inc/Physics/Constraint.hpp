#pragma once

namespace Physics {

	class Object;
	class Constraint {
	public:

		enum class ConstraintType {
			SPRING,
			JOINT
		};

		Constraint(Object* objA, Object* objB, ConstraintType type);
		virtual ~Constraint();

		virtual void FixedUpdate() = 0;

		inline const void GetConnections(Object* objA, Object* objB) const { objA = m_ObjA; objB = m_ObjB; }
		inline ConstraintType GetType() { return m_Type; }

	protected:

		Object* m_ObjA;
		Object* m_ObjB;

		ConstraintType m_Type;

	};

}