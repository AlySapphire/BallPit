#pragma once

namespace Physics {

	class Object;
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

	protected:

		ColliderType m_Type;

	};

}



