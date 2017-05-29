#pragma once

#include "Constraint.hpp"

namespace Physics {

	class Object;
	class Spring : public Constraint {
	public:
		Spring(Object* objA, Object* objB);
		Spring(Object* objA, Object* objB, float length, float stiffness, float friction);
		virtual ~Spring();

		virtual void FixedUpdate();

		//Getters
		inline const float GetLength() const { return m_Length; }
		inline const float GetStiffness() const { return m_Stiffness; }
		inline const float GetFriction() const { return m_Friction; }

		//Setters
		inline void SetStiffness(float stiff) { m_Stiffness = stiff; }
		inline void SetFriction(float fric) { m_Friction = fric; }

	protected:

		float m_Length;
		float m_Stiffness;
		float m_Friction;

	};

}