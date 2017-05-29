#pragma once

#include <map>
#include <glm/vec4.hpp>

namespace Physics {

	class Scene;
	class Object;
	class Spring;
	class GizmosRenderer {
	public:
		GizmosRenderer();
		virtual ~GizmosRenderer();

		void Draw(Scene* scene);

		struct RenderInfo {
			glm::vec4 color = glm::vec4(1);
		};

		inline RenderInfo* GetRenderInfo(Object* obj) { return &m_ObjectRenderInfo[obj]; }
		inline RenderInfo* GetRenderInfo(Spring* spring) { return &m_SpringRenderInfo[spring]; }

	protected:

		std::map<Object*, RenderInfo> m_ObjectRenderInfo;
		std::map<Spring*, RenderInfo> m_SpringRenderInfo;

		void RenderGizmosObjects(Scene* scene);
		void RenderGizmosConstraints(Scene* scene);

	};

}