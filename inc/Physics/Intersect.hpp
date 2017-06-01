#pragma once

#include <glm/vec3.hpp>

namespace Physics {

	enum class CollisionType {
		SPHERE2SPHERE,
		SPHERE2AABB,
		AABB2SPHERE,
		AABB2AABB
	};

	struct IntersectData {
		glm::vec3 collisionVector;
		CollisionType intersectionType;
	};
}