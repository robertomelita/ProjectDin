#include <btBulletDynamicsCommon.h>
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "worldPhysics.h"

worldPyshics::worldPyshics(){
    this->collisionConfiguration = new btDefaultCollisionConfiguration();
    this->dispatcher = new btCollisionDispatcher(collisionConfiguration);
    this->overlappingPairCache = new btDbvtBroadphase();
    this->solver = new btSequentialImpulseConstraintSolver;
    this->dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    this->dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

void worldPyshics::addRigidBody(btRigidBody *body){
    this->dynamicsWorld->addRigidBody(body);
}
void worldPyshics::stepSimulation(){
    this->dynamicsWorld->stepSimulation(1.f / 120.f*2, 10);
}
void worldPyshics::del(){
    for (int j = 0; j < this->collisionShapes.size(); j++){
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}
	delete this->dynamicsWorld;
	delete this->solver;
	delete this->overlappingPairCache;
	delete this->dispatcher;
	delete this->collisionConfiguration;
	this->collisionShapes.clear();
}
btDiscreteDynamicsWorld* worldPyshics::getDynamicWorld(){
    return this->dynamicsWorld;
}

btAlignedObjectArray<btCollisionShape*> worldPyshics::getCollisionShapes(){
    return this->collisionShapes;
}