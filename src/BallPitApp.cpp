#include "BallPitApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Rendering/Camera.h"

#include "Physics/PhysicsObject.hpp"
#include "Physics/SphereCollider.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

BallPitApp::BallPitApp() : m_Camera(nullptr) {

}

BallPitApp::~BallPitApp() {

	m_Camera = nullptr;

}

bool BallPitApp::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	//Initialize camera
	m_Camera = new Camera();
	m_Camera->SetProjection(glm::radians(45.0f), (float)getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);
	m_Camera->SetPosition(glm::vec3(5, 10, 5));
	m_Camera->Lookat(glm::vec3(0, 0, 0));

	for(int i = -3; i < 3; i++) {
		Physics::Object* obj = new Physics::Object();
		obj->SetPosition(glm::vec3(i, 5, 0));
		obj->SetCollider(new Physics::SphereCollider(0.25f));
		m_Objects.push_back(obj);
	}

	return true;
}

void BallPitApp::shutdown() {

	Gizmos::destroy();

	if(m_Camera != nullptr)		delete m_Camera;

	for(auto iter : m_Objects) {
		delete iter;
	}
	m_Objects.clear();

}

void BallPitApp::update(float deltaTime) {

	m_Camera->Update(deltaTime);

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	for(auto iter : m_Objects) {
		iter->ApplyForce(glm::vec3(0, -9.8, 0));
		iter->FixedUpdate();
	}

}

void BallPitApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	DrawGrid();

	for(auto iter : m_Objects) {
		Physics::SphereCollider* sCol = (Physics::SphereCollider*)iter->GetCollider();
		Gizmos::addSphere(sCol->GetPosition(), sCol->GetRadius(), 8, 8, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	Gizmos::draw(m_Camera->GetProjectionView());
}

void BallPitApp::DrawGrid() {

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for(int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

}
