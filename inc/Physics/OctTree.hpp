#pragma once

#include <vector>
#include <glm/vec3.hpp>

namespace Physics {

	class Object;
	class OctTree {
	public:

		OctTree();
		OctTree(const glm::vec3& regionMin, const glm::vec3& regionMax);
		OctTree(const glm::vec3& regionMin, const glm::vec3& regionMax, const std::vector<Object*>& objects);
		virtual ~OctTree();

		void Update();
		void Insert(Object* obj);

	protected:

		void BuildTree();
		void UpdateTree();
		void DoInsert(Object* obj);
		void FindEnclosingCube();

		OctTree* m_parent;
		std::vector<OctTree*> m_children;

		//Bitmask for tracking child nodes
		unsigned char m_activeNodes = 0;

		const int MIN_SIZE = 1;

		glm::vec3 m_regionMin;
		glm::vec3 m_regionMax;

		int m_maxLifespan = 8;
		int m_curLife = -1;

		static bool m_treeReady;
		static bool m_treeBuilt;

		std::vector<Object*> m_objects;

		static std::vector<Object*> m_pendingInsertion;

	};
}


