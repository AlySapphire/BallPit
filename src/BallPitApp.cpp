#include "BallPitApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Rendering/Camera.h"
#include "Rendering/GizmosRenderer.hpp"

#include "Physics/PhysicsObject.hpp"
#include "Physics/SphereCollider.hpp"
#include "Physics/PhysicsScene.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

BallPitApp::BallPitApp() : m_Camera(nullptr), m_PhysicsScene(nullptr), m_GizmosRenderer(nullptr) {

}

BallPitApp::~BallPitApp() {

	m_Camera = nullptr;
	m_PhysicsScene = nullptr;
	m_GizmosRenderer = nullptr;

}

bool BallPitApp::startup() {

	srand(unsigned(time(NULL)));
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	//Initialize camera
	m_Camera = new Camera();
	m_Camera->SetProjection(glm::radians(45.0f), (float)getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);
	m_Camera->SetPosition(glm::vec3(5, 10, 5));
	m_Camera->Lookat(glm::vec3(0, 0, 0));

	//Initialize Scene and Renderer
	m_PhysicsScene = new Physics::Scene();
	m_GizmosRenderer = new Physics::GizmosRenderer();

	//Add Objects to the scene
	for(int i = -3; i < 3; i++) {
		Physics::Object* obj = new Physics::Object();
		obj->SetPosition(glm::vec3(i, 5, 0));
		obj->SetCollider(new Physics::SphereCollider(0.25f));
		m_GizmosRenderer->GetRenderInfo(obj)->color = glm::vec4(
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			1.0f
		);

		m_PhysicsScene->AttachObject(obj);
	}

	//Give the scene gravity
	m_PhysicsScene->SetGravity(glm::vec3(0, -9.8f, 0));

	return true;
}

void BallPitApp::shutdown() {

	Gizmos::destroy();

	if(m_Camera != nullptr)			delete m_Camera;
	if(m_PhysicsScene != nullptr)	delete m_PhysicsScene;
	if(m_GizmosRenderer != nullptr)	delete m_GizmosRenderer;
	
}

void BallPitApp::update(float deltaTime) {

	m_Camera->Update(deltaTime);

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	//Shoot ball
	if(input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT) && input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT)) {
		float shotSpeed = 20.0f;
		Physics::Object* obj = new Physics::Object();
		obj->SetPosition(m_Camera->GetPosition());
		obj->SetVelocity(m_Camera->GetForward() * shotSpeed);
		obj->SetCollider(new Physics::SphereCollider(0.25f));
		obj->SetMass(20.0f);
		obj->SetBounciness(5);

		m_GizmosRenderer->GetRenderInfo(obj)->color = glm::vec4(
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			1.0f
		);

		m_PhysicsScene->AttachObject(obj);
	}

	m_PhysicsScene->FixedUpdate();

}

void BallPitApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	DrawGrid();

	//Render our objects
	m_GizmosRenderer->Draw(m_PhysicsScene);

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
