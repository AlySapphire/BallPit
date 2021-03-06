#pragma once

#include "Intersect.hpp"

namespace Physics {

	class Object;
	class SphereCollider;
	class AABBCollider;
	class Collider {
	public:

		enum class ColliderType {
			NONE,
			SPHERE,
			AABB
		};

		Collider(ColliderType type);
		virtual ~Collider();
		
		inline const ColliderType GetType() const { return m_Type; }

		virtual void Transform(Object* obj) {  }

		bool Intersects(Collider* other, IntersectData* intersection);

		//TODO: Move this into some sort of collision class
		static bool Sphere2Sphere(SphereCollider* objA, SphereCollider* objB, IntersectData* intersection);
		static bool Sphere2AABB(SphereCollider* objA, AABBCollider* objB, IntersectData* intersection);
		static bool AABB2Sphere(AABBCollider* objA, SphereCollider* objB, IntersectData* intersection);
		static bool AABB2AABB(AABBCollider* objA, AABBCollider* objB, IntersectData* intersection);

	protected:

		ColliderType m_Type;

	};

}



