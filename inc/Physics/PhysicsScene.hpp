#pragma once

#include <vector>
#include <map>
#include "Intersect.hpp"

namespace Physics {

	class Object;
	class Constraint;
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
		inline const std::vector<Constraint*>& GetConstraints() const { return m_Constraints; }

		//Setters
		inline void SetGravity(const glm::vec3& gravity) { m_Gravity = gravity; }

		void AttachObject(Object* obj);
		void RemoveObject(Object* obj);
		
		void AttachConstraint(Constraint* con);
		void RemoveConstraint(Constraint* con);

		inline bool IsInCollision(Object* obj) { return m_InCollisionLookup[obj]; }

	protected:

		friend class Tree;

		//void DetectCollisions();
		//void ResolveCollisions();

		std::vector<Object*> m_Objects;
		std::vector<Constraint*> m_Constraints;

		std::vector<CollisionInfo> m_CollisionPairs;
		std::map<Object*, bool> m_InCollisionLookup;

		glm::vec3 m_GlobalForce;
		glm::vec3 m_Gravity;

		Tree* m_tree;

	};

}