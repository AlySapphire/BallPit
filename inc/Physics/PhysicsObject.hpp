#pragma once

#include <glm/vec3.hpp>

namespace Physics {

	class Collider;
	class Object {
	public:
		Object();
		virtual ~Object();

		virtual void FixedUpdate();

		void ApplyForce(const glm::vec3& a_Force);

		//Getters
		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const glm::vec3& GetVelocity() const { return m_Velocity; }
		inline const glm::vec3& GetMaxVelocity() const { return m_MaxVelocity; }
		inline const glm::vec3& GetAcceleration() const { return m_Acceleration; }
		inline const float GetMass() const { return m_Mass; }
		inline const float GetFriction() const { return m_Friction; }
		inline const float GetBounciness() const { return m_Bounciness; }
		inline const bool GetRigid() const { return m_Rigid; }
		Collider* GetCollider();

		//Setters
		void SetPosition(const glm::vec3& a_Pos);
		inline void SetVelocity(const glm::vec3& a_Vel) { m_Velocity = a_Vel; }
		inline void SetMaxVelocity(const glm::vec3& a_MaxVel) { m_MaxVelocity = a_MaxVel; }
		inline void SetAcceleration(const glm::vec3& a_Acc) { m_Acceleration = a_Acc; }
		inline void SetMass(float a_Mass) { m_Mass = a_Mass; }
		inline void SetFriction(float a_Fric) { m_Friction = a_Fric; }
		inline void SetBounciness(float a_Bounce) { m_Bounciness = a_Bounce; }
		inline void SetRigid(bool a_Rigid) { m_Rigid = a_Rigid; }
		void SetCollider(Collider* coll);

	protected:

		void UpdateTransform();

		glm::vec3 m_Position;
		glm::vec3 m_Velocity;
		glm::vec3 m_MaxVelocity;
		glm::vec3 m_Acceleration;

		float m_Mass;
		float m_Friction;
		float m_Bounciness;

		Collider* m_Collider;

		bool m_Rigid = false;

	};


}