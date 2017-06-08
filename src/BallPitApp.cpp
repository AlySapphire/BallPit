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
#include "Physics/Spring.hpp"
#include "Physics/AABBCollider.hpp"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

float GetDistance(const glm::vec3& posA, const glm::vec3& posB) {
	return glm::distance(posA, posB);
}

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
	Gizmos::create(100000, 100000, 100000, 100000);

	//Initialize camera
	m_Camera = new Camera();
	m_Camera->SetProjection(glm::radians(45.0f), (float)getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);
	m_Camera->SetPosition(glm::vec3(5, 10, 5));
	m_Camera->Lookat(glm::vec3(0, 0, 0));

	//Initialize Scene and Renderer
	m_PhysicsScene = new Physics::Scene();
	m_GizmosRenderer = new Physics::GizmosRenderer();

	//Add Objects to the scene
	for(int x = -5; x < 5; x++) {
		//for(int y = 1; y < 4; y++) {
			for(int z = -5; z < 5; z++) {
				Physics::Object* obj = new Physics::Object();
				obj->SetPosition(glm::vec3(x + 0.5f, 2, z + 0.5f));
				obj->SetCollider(new Physics::SphereCollider(0.5f));
				m_GizmosRenderer->GetRenderInfo(obj)->color =
					glm::vec4(
						rand() % 255 / 255.0f,
						rand() % 255 / 255.0f,
						rand() % 255 / 255.0f,
						1.0f
					);

				m_PhysicsScene->AttachObject(obj);
			}
		//}
	}

	float border = 7.5f;
	float height = 2.0f;

	//Create AABB container
	Physics::Object* leftBorder = new Physics::Object();
	leftBorder->SetPosition(glm::vec3(-border - 4, 1, 0));
	leftBorder->SetCollider(new Physics::AABBCollider(glm::vec3(5.0f, height + 0.5f, border + 4.5f)));
	leftBorder->SetRigid(true);
	m_GizmosRenderer->GetRenderInfo(leftBorder)->color =
		glm::vec4(
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			1.0f
		);
	m_PhysicsScene->AttachObject(leftBorder);

	Physics::Object* rightBorder = new Physics::Object();
	rightBorder->SetPosition(glm::vec3(border + 4, 1, 0));
	rightBorder->SetCollider(new Physics::AABBCollider(glm::vec3(5.0f, height + 0.5f, border + 4.5f)));
	rightBorder->SetRigid(true);
	m_GizmosRenderer->GetRenderInfo(rightBorder)->color =
		glm::vec4(
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			1.0f
		);
	m_PhysicsScene->AttachObject(rightBorder);

	Physics::Object* topBorder = new Physics::Object();
	topBorder->SetPosition(glm::vec3(0, 1, -border - 4));
	topBorder->SetCollider(new Physics::AABBCollider(glm::vec3(border + 4.5f, height + 0.5f, 5.0f)));
	topBorder->SetRigid(true);
	m_GizmosRenderer->GetRenderInfo(topBorder)->color =
		glm::vec4(
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			1.0f
		);
	m_PhysicsScene->AttachObject(topBorder);

	Physics::Object* bottomBorder = new Physics::Object();
	bottomBorder->SetPosition(glm::vec3(0, 1, border + 4));
	bottomBorder->SetCollider(new Physics::AABBCollider(glm::vec3(border + 4.5f, height + 0.5f, 5.0f)));
	bottomBorder->SetRigid(true);
	m_GizmosRenderer->GetRenderInfo(bottomBorder)->color =
		glm::vec4(
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			rand() % 255 / 255.0f,
			1.0f
		);
	m_PhysicsScene->AttachObject(bottomBorder);

	//Test AABB
	//Physics::Object* testAABB = new Physics::Object();
	//testAABB->SetPosition(glm::vec3(0, 1, 4));
	//testAABB->SetCollider(new Physics::AABBCollider(glm::vec3(0.5f)));
	//testAABB->SetRigid(true);
	//m_GizmosRenderer->GetRenderInfo(testAABB)->color =
	//	glm::vec4(
	//		rand() % 255 / 255.0f,
	//		rand() % 255 / 255.0f,
	//		rand() % 255 / 255.0f,
	//		1.0f
	//	);
	//m_PhysicsScene->AttachObject(testAABB);

	
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
		obj->SetMass(10.0f);
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
