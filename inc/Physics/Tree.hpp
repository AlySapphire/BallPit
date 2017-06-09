#pragma once

#include <vector>
#include <glm/vec3.hpp>

namespace Physics {

	class Object;
	class Scene;
	class Tree {
	public:

		Tree();
		Tree(Tree* parent);
		Tree(const std::vector<Object*> & objects);
		Tree(Tree* parent, const std::vector<Object*>& objects, const glm::vec3& regionDir);
		virtual ~Tree();

		bool Insert(Object* obj);

		void BuildTree();
		void Update(Scene* scene);

	protected:

		void DetectCollisions(Scene* scene, std::vector<Object*>* parentObjs = nullptr);
		void ResolveCollisions(Scene* scene);

		bool fit(Object* obj, const glm::vec3& dir);

		std::vector<Tree*> m_childNodes;
		Tree* m_parent;

		std::vector<Object*> m_objects;

		glm::vec3 m_regionDir;

		static bool m_treebuilt;

	};

}