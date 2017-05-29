#pragma once

#include <vector>
#include <map>
#include "Intersect.hpp"

namespace Physics {

	class Object;
	class Scene {
	public:

		struct CollisionInfo {
			Object* objA;
			Object* objB;

			IntersectData intersection;
		};

		Scene();
		virtual ~Scene();

		void FixedUpdate();

		inline void ApplyGlobalForce(const glm::vec3& force) { m_GlobalForce += force; }

		//Getters
		inline const glm::vec3& GetGravity() const { return m_Gravity; }
		inline const std::vector<Object*>& GetObjects() const { return m_Objects; }

		//Setters
		inline void SetGravity(const glm::vec3& gravity) { m_Gravity = gravity; }

		void AttatchObject(Object* obj);
		void RemoveObject(Object* obj);

		inline bool IsInCollision(Object* obj) { return m_InCollisionLookup[obj]; }

	protected:

		void DetectCollisions();
		void ResolveCollisions();

		std::vector<Object*> m_Objects;

		std::vector<CollisionInfo> m_CollisionPairs;
		std::map<Object*, bool> m_InCollisionLookup;

		glm::vec3 m_GlobalForce;
		glm::vec3 m_Gravity;

	};

}