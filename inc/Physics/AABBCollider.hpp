#pragma once

#include "Collider.hpp"

namespace Physics {

	class AABBCollider : public Collider {
	public:
		AABBCollider();
		AABBCollider(const glm::vec3& centre, const glm::vec3& extents);
		virtual ~AABBCollider();

		//GETTERS
		inline const glm::vec3& GetCentre() const { return m_Centre; }
		inline const glm::vec3& GetExtents() const { return m_Extents; }

		virtual void Transform(Object* obj);

	protected:
		glm::vec3 m_Centre;
		glm::vec3 m_Extents;

	};

}


