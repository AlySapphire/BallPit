#pragma once

#include "Collider.hpp"
#include <glm/vec3.hpp>

namespace Physics {

	class SphereCollider : public Collider {
	public:
		SphereCollider();
		SphereCollider(float radius);
		virtual ~SphereCollider();

		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const float GetRadius() const { return m_Radius; }

		virtual void Transform(Object* obj);

	protected:

		glm::vec3 m_Position;
		float m_Radius;

	};

}