#pragma once

#include <map>
#include <glm/vec4.hpp>

namespace Physics {

	class Scene;
	class Object;
	class GizmosRenderer {
	public:
		GizmosRenderer();
		virtual ~GizmosRenderer();

		void Draw(Scene* scene);

		struct RenderInfo {
			glm::vec4 color = glm::vec4(1);
		};

		inline RenderInfo* GetRenderInfo(Object* obj) { return &m_ObjectRenderInfo[obj]; }

	protected:

		std::map<Object*, RenderInfo> m_ObjectRenderInfo;

		void RenderGizmosObjects(Scene* scene);

	};

}