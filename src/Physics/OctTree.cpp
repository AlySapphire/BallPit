#include "Physics/OctTree.hpp"
#include "Physics/PhysicsObject.hpp"
#include "Physics/AABBCollider.hpp"
#include "Physics/SphereCollider.hpp"

#include <glm/geometric.hpp>

using std::vector;

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

		if(!m_treeReady)
			UpdateTree();

		//Start a countdown timer for leaf nodes with no objects
		//If the timer reaches zero then trim the leaf. However if we reuse the leaf before death then the lifespan should be doubled
		//This gives us a "frequency" effect and lets us avoid inefficient memory allocating and deallocating
		if(m_objects.size() == 0) {
			if(!m_hasChildren) {
				if(m_curLife == -1)
					m_curLife = m_maxLifespan;
				else if(m_curLife > 0)
					m_curLife--;
			}

		} else {
			if(m_curLife != -1) {
				if(m_maxLifespan <= 64)
					m_maxLifespan *= 2;
				m_curLife = -1;
			}
		}
			
		std::vector<Object*> movedObjects;

		//Update all objects in the current node
		for(auto obj : m_objects) {
			if(obj->FixedUpdate())
				movedObjects.push_back(obj);
		}

		//Recursively update any child nodes
		for(int flags = m_activeNodes, i = 0; flags > 0; flags >>= 1, i++) {
			if((flags & 1) == 1)
				m_children[i]->Update();
		}

		//If an object moved, move it up to the closest containing parent and then work our way back down
		for(auto obj : movedObjects) {

			OctTree* current = this;

			//Determine how far up the tree we must traverse to reinsert the object
			while(!current->Contains(obj, m_regionMin, m_regionMax)) {
				if(current->m_parent != nullptr)
					current = current->m_parent;
				else break;
			}
				
			auto find = std::find(m_objects.begin(), m_objects.end(), obj);
			if(find != m_objects.end()) {
				m_objects.erase(find);
				current->DoInsert(obj);
			}

		}

		//Trim any dead branches
		for(int flags = m_activeNodes, i = 0; flags > 0; flags >>= 1, i++) {
			if((flags & 1) == 1 && m_children[i]->m_curLife == 0) {
				delete m_children[i];
				m_children[i] = nullptr;
				m_activeNodes ^= (unsigned char)(1 << i);
			}
		}

	}

	void OctTree::Insert(Object* obj) {
		m_pendingInsertion.push_back(obj);
		m_treeReady = false;
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

		this->m_hasChildren = true;

		glm::vec3 half = dimensions / 2.0f;
		glm::vec3 center = m_regionMin + half;

		//Create sub-divided regions for each octant
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

		//This list contains objects that will be passed down to child nodes should they fit
		vector<Object*>octList[8];
		//Objects that will be removed from parent if they fit
		vector<Object*>delist;

		for(auto iter : m_objects) {
			for(int region = 0; region < 8; region++) {
				if(Contains(iter, octantMin[region], octantMax[region])) {
					octList[region].push_back(iter);
					delist.push_back(iter);
					break;
				}
			}
		}
		
		//Remove objects if they are marked for delisting
		for(auto iter : delist) {
			auto find = std::find(m_objects.begin(), m_objects.end(), iter);
			if(find != m_objects.end())
				m_objects.erase(find);
		}

		//Create child nodes where there are items contained in the bounding region
		for(unsigned int i = 0; i < 8; i++) {
			m_children.push_back(CreateNode(octantMin[i], octantMax[i], octList[i]));
			m_activeNodes |= (unsigned char)(1 << i);
			m_children.back()->BuildTree();
		}

		m_treeReady = true;
		m_treeBuilt = true;

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

		//Make sure we don't have to insert an object deeper than it needs to go
		if(m_objects.size() <= 1 && m_activeNodes == 0) {
			m_objects.push_back(obj);
			return;
		}

		glm::vec3 dimensions = m_regionMax - m_regionMin;

		//Check to see if dimensions are required size
		if(dimensions.x <= MIN_SIZE && dimensions.y <= MIN_SIZE && dimensions.z <= MIN_SIZE) {
			m_objects.push_back(obj);
			return;
		}
		
		glm::vec3 half = dimensions / 2.0f;
		glm::vec3 center = m_regionMin + half;

		//Create sub-divided regions for each octant
		glm::vec3 octantMin[8] = {
			{ m_regionMin.x, m_regionMin.y, m_regionMin.z },{ center.x, m_regionMin.y, m_regionMin.z },
			{ center.x, m_regionMin.y, center.z },{ m_regionMin.x, m_regionMin.y, center.z },
			{ m_regionMin.x, center.y, m_regionMin.z },{ center.x, center.y, m_regionMin.z },
			{ center.x, center.y, center.z },{ m_regionMin.x, center.y, center.z }
		};
		glm::vec3 octantMax[8] = {
			{ center.x, center.y, center.z },{ m_regionMax.x, center.y, center.z },
			{ m_regionMax.x, center.y, m_regionMax.z },{ center.x, center.y, m_regionMax.z },
			{ center.x, m_regionMax.y, center.z },{ m_regionMax.x, m_regionMax.y, center.z },
			{ m_regionMax.x, m_regionMax.y, m_regionMax.z },{ center.x, m_regionMax.y, m_regionMax.z }
		};

		if(!m_children.empty()) {
			for(unsigned int i = 0; i < m_children.size(); i++) {
				if(m_children[i] == nullptr)	continue;
				octantMin[i] = m_children[i]->m_regionMin;
				octantMax[i] = m_children[i]->m_regionMax;
			}
		}

		if(Contains(obj, m_regionMin, m_regionMax)) {
			bool found = false;
			for(int i = 0; i < 8; i++) {
				if(Contains(obj, octantMin[i], octantMax[i])) {
					if(m_children[i] != nullptr)
						m_children[i]->Insert(obj);
					else {
						m_children[i] = CreateNode(octantMin[i], octantMax[i], obj);
						m_activeNodes |= (unsigned char)(1 << i);
					}
					found = true;
					break;
				}
			}
			if(!found)
				m_objects.push_back(obj);
		} else {
			FindEnclosingCube();
			BuildTree();
		}
		

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

				return(boxMin.x > minRegion.x && boxMax.x < maxRegion.x) &&
					  (boxMin.y > minRegion.y && boxMax.y < maxRegion.y) &&
					  (boxMin.z > minRegion.z && boxMin.z < maxRegion.z);

			}
			case Collider::ColliderType::SPHERE: {
				//Cast to Sphere collider
				SphereCollider* sc = (SphereCollider*)obj->GetCollider();

				//Cache sphere values
				auto& spherePos = sc->GetPosition();
				float sphereRadius = sc->GetRadius();

				return(spherePos.x - sphereRadius > minRegion.x && spherePos.x + sphereRadius < maxRegion.x)&&
					  (spherePos.y - sphereRadius > minRegion.y && spherePos.y + sphereRadius < maxRegion.y)&&
					  (spherePos.z - sphereRadius > minRegion.z && spherePos.z + sphereRadius < maxRegion.z);

			}
				
		}

		return false;
	}

	OctTree * OctTree::CreateNode(const glm::vec3 & regionMin, const glm::vec3 & regionMax, std::vector<Object*>& objs) {

		OctTree* node = new OctTree(regionMin, regionMax, objs);
		node->m_parent = this;

		return node;

	}

	OctTree * OctTree::CreateNode(const glm::vec3 & regionMin, const glm::vec3 & regionMax, Object * obj) {
		vector<Object*> temp = { obj };

		OctTree* node = new OctTree(regionMin, regionMax, temp);
		node->m_parent = this;

		return node;

	}

	void OctTree::DetectCollisions(std::vector<Object*>* parentObjs) {

		//TODO: Implement detection algorithm

		if(parentObjs != nullptr) {
			for(auto parent : *parentObjs) {
				for(auto obj : m_objects) {

				}
			}
		}

	}

}