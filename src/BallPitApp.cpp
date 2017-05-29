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
	//for(int i = -3; i < 3; i++) {
	//	Physics::Object* obj = new Physics::Object();
	//	obj->SetPosition(glm::vec3(i, 5, 0));
	//	obj->SetCollider(new Physics::SphereCollider(0.25f));
	//	m_GizmosRenderer->GetRenderInfo(obj)->color = glm::vec4(
	//		rand() % 255 / 255.0f,
	//		rand() % 255 / 255.0f,
	//		rand() % 255 / 255.0f,
	//		1.0f
	//	);
	//
	//	m_PhysicsScene->AttachObject(obj);
	//}

	const int maxX = 4;
	const int maxY = 10;
	const int maxZ = 4;

	Physics::Object* blob[maxX][maxY][maxZ];

	for(int x = 0; x < maxX; x++) {
		for(int y = 0; y < maxY; y++) {
			for(int z = 0; z < maxZ; z++) {
				blob[x][y][z] = new Physics::Object();
				blob[x][y][z]->SetPosition(glm::vec3(x, y + 1, z));
				blob[x][y][z]->SetCollider(new Physics::SphereCollider(0.25f));

				m_GizmosRenderer->GetRenderInfo(blob[x][y][z])->color = glm::vec4(
					rand() % 255 / 255.0f,
					rand() % 255 / 255.0f,
					rand() % 255 / 255.0f,
					1.0f
				);

				m_PhysicsScene->AttachObject(blob[x][y][z]);

			}
		}
	}

	float stiffness = 745.0f;
	float friction = 0.4f;

	for(int x = 0; x < maxX; x++) {
		for(int y = 0; y < maxY; y++) {
			for(int z = 0; z < maxZ; z++) {

				bool xNext = (x + 1 < maxX);
				bool yNext = (y + 1 < maxY);
				bool zNext = (z + 1 < maxZ);
				bool xBefore = (x - 1 >= 0);
				bool yBefore = (y - 1 >= 0);
				bool zBefore = (z - 1 >= 0);

				if(zNext) {

					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x][y][z + 1], GetDistance(blob[x][y][z]->GetPosition(),
																													  blob[x][y][z + 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}
				if(yNext) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x][y + 1][z], GetDistance(blob[x][y][z]->GetPosition(),
																													  blob[x][y + 1][z]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}
				if(xNext) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x + 1][y][z], GetDistance(blob[x][y][z]->GetPosition(),
																													  blob[x + 1][y][z]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}

				if(xNext && yNext) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x + 1][y + 1][z],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x + 1][y + 1][z]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}

				if(zNext && yNext) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x][y + 1][z + 1],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x][y + 1][z + 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}
				if(xNext && zNext) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x + 1][y][z + 1],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x + 1][y][z + 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}

				if(xBefore && zBefore) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x - 1][y][z - 1],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x - 1][y][z - 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}
				if(yBefore && zBefore) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x][y - 1][z - 1],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x][y - 1][z - 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}
				if(xBefore && yBefore) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x - 1][y - 1][z],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x - 1][y - 1][z]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}

				if(xNext && yNext && zNext) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x + 1][y + 1][z + 1],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x + 1][y + 1][z + 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}
				if(xNext && yBefore && zNext) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x + 1][y - 1][z + 1],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x + 1][y - 1][z + 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}
				if(xNext && yNext && zBefore) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x + 1][y + 1][z - 1],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x + 1][y + 1][z - 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}
				if(xNext && yBefore & zBefore) {
					Physics::Spring* spring = new Physics::Spring(blob[x][y][z], blob[x + 1][y - 1][z - 1],
																		GetDistance(blob[x][y][z]->GetPosition(),
																					blob[x + 1][y - 1][z - 1]->GetPosition()),
																		stiffness, friction);
					m_GizmosRenderer->GetRenderInfo(spring)->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
					m_PhysicsScene->AttachConstraint(spring);
				}

			}
		}
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
