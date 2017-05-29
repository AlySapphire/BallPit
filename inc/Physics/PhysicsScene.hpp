#pragma once

#include <glm/vec3.hpp>
#include <vector>

namespace Physics {

	class Object;
	class Scene {
	public:
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

	protected:

		std::vector<Object*> m_Objects;

		glm::vec3 m_GlobalForce;
		glm::vec3 m_Gravity;

	};

}