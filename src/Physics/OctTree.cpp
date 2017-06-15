#include "Physics/OctTree.hpp"
#include "Physics/PhysicsObject.hpp"
#include "Physics/AABBCollider.hpp"
#include "Physics/SphereCollider.hpp"

#include <glm/geometric.hpp>

namespace Physics {

	bool OctTree::m_treeBuilt = false;
	bool OctTree::m_treeReady = false;
	std::vector<Object*> OctTree::m_pendingInsertion;

	OctTree::OctTree() : m_parent(nullptr) {
	}

	OctTree::OctTree(const glm::vec3 & regionMin, const glm::vec3 & regionMax) : m_regionMin(regionMin), m_regionMax(regionMax), m_parent(nullptr) {
	}

	OctTree::OctTree(const glm::vec3 & regionMin, const glm::vec3 & regionMax, const std::vector<Object*>& objects) : m_regionMin(regionMin), m_regionMax(regionMax), m_objects(objects), m_parent(nullptr) {
	}

	OctTree::~OctTree() {

		//Clean up child nodes
		for(auto child : m_children)
			delete child;
		m_children.clear();

	}

	void OctTree::Update() {

		

	}

	void OctTree::Insert(Object* obj) {
		m_pendingInsertion.push_back(obj);
	}

	void OctTree::BuildTree() {

		//Terminate recursion if we're a leaf node
		if(m_objects.size() <= 1)	return;

		glm::vec3 dimensions = m_regionMax - m_regionMin;

		if(dimensions == glm::vec3()) {
			FindEnclosingCube();
			dimensions = m_regionMax - m_regionMin;
		}

		//Check to see if dimensions are required size
		if(dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE)
			return;

		glm::vec3 half = dimensions / 2.0f;
		glm::vec3 center = m_regionMin + half;

		glm::vec3 octantMin[8] = {
			{ m_regionMin.x, m_regionMin.y, m_regionMin.z }, { center.x, m_regionMin.y, m_regionMin.z},
			{ center.x, m_regionMin.y, center.z }, { m_regionMin.x, m_regionMin.y, center.z }, 
			{ m_regionMin.x, center.y, m_regionMin.z }, { center.x, center.y, m_regionMin.z },
			{ center.x, center.y, center.z }, { m_regionMin.x, center.y, center.z }
		};

		glm::vec3 octantMax[8] = {
			{ center.x, center.y, center.z }, { m_regionMax.x, center.y, center.z },
			{ m_regionMax.x, center.y, m_regionMax.z }, { center.x, center.y, m_regionMax.z },
			{ center.x, m_regionMax.y, center.z }, { m_regionMax.x, m_regionMax.y, center.z },
			{ m_regionMax.x, m_regionMax.y, m_regionMax.z }, { center.x, m_regionMax.y, m_regionMax.z }
		};

	}

	void OctTree::UpdateTree() {

		if(!m_treeBuilt) {
			while(!m_pendingInsertion.empty()) {
				m_objects.push_back(m_pendingInsertion.back());
				m_pendingInsertion.pop_back();
			}
			BuildTree();

		} else {
			while(!m_pendingInsertion.empty()) {
				DoInsert(m_pendingInsertion.back());
				m_pendingInsertion.pop_back();
			}
		}

		m_treeReady = true;
	}

	void OctTree::DoInsert(Object* obj) {

	}

	void OctTree::FindEnclosingCube() {

		glm::vec3 offset = glm::vec3(0) - m_regionMin;
		m_regionMin += offset;
		m_regionMax += offset;

		int highX = (int)glm::ceil(glm::max(glm::max(m_regionMax.x, m_regionMax.y), m_regionMax.z));

		//See if our cube dimension is already at a power of 2. If it is, we don't have to do any work.
		for(int bit = 0; bit < 32; bit++) {
			if(highX == 1 << 2) {
				m_regionMax = glm::vec3(highX, highX, highX);

				m_regionMin -= offset;
				m_regionMax -= offset;
				return;
			}
		}

		//We have a cube with non-power of 2 dimensions so find the next highest value of 2
		auto upperPowerOfTwo = [](int numToPow)->int {
			numToPow--;
			numToPow |= numToPow >> 1;
			numToPow |= numToPow >> 2;
			numToPow |= numToPow >> 4;
			numToPow |= numToPow >> 8;
			numToPow |= numToPow >> 16;
			numToPow++;
			return numToPow;
		};

		int x = upperPowerOfTwo(highX);

		m_regionMax = glm::vec3(x);
		m_regionMin -= offset;
		m_regionMax -= offset;

	}

	bool OctTree::Contains(Object * obj, const glm::vec3 & minRegion, const glm::vec3 & maxRegion) {

		switch(obj->GetCollider()->GetType()) {
			case Collider::ColliderType::AABB: {
				//Cast to AABB collider
				AABBCollider* ac = (AABBCollider*)obj->GetCollider();
				
				//Cache AABB values
				auto& boxCenter = ac->GetCentre();
				auto& boxExtents = ac->GetExtents();

				//Calculate min and max of AABB
				glm::vec3 boxMin = boxCenter - boxExtents;
				glm::vec3 boxMax = boxCenter + boxExtents;



			}
				break;
				
		}

		return false;
	}

}