    /******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries for separate legal notices                          |
|******************************************************************************|
| This demo uses the Assimp library to load a mesh from a file, and supports   |
| many formats. The library is VERY big and complex. It's much easier to write |
| a simple Wavefront .obj loader. I have code for this in other demos. However,|
| Assimp will load animated meshes, which will we need to use later, so this   |
| demo is a starting point before doing skinning animation                     |
\******************************************************************************/
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
#include "gl_utils.h"
#include "stb_image.h"
#include "tools.h"
#include "malla.h"
#include "cubo.h"
#include "enemy.h"
#include "protagonist.h"
#include "suelo.h"
#include "input.h"
#include "skybox.h"
#include "sound.h"
#include "worldPhysics.h"
#include "gltext.h"
#include "GLDebugDrawer.hpp"

#define GL_LOG_FILE "log/gl.log"
using namespace std;

int g_gl_width = 1280;
int g_gl_height = 720;
GLFWwindow* g_window = NULL;

int distancia = 12.0f;
float yawPersonaje = 90.0f;
int speedLimit = 12;

glm::vec3 impulso = glm::vec3(0,0,0);
glm::vec3 jump = glm::vec3(0,0,0);
glm::vec3 posObj = glm::vec3(0.0f,-8.0f,-2.0f);
btQuaternion rotObj;

// camera
glm::vec3 cameraPos   = posObj + glm::vec3(0.0f, 0.5f, distancia);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLuint shader_programme;

//mallas
protagonist *sword;
suelo *piso;
suelo *piso2;
suelo *sala;
suelo *castillo;
suelo *espada;
suelo *arbolito;
worldPhysics* world;
enemy *key;
cubo *cubo1;
cubo *cubo2;
suelo* terrenoExterior;
sound *snd_01 = new sound((const char*)"audio/rito2.wav");
sound *snd_02 = new sound((const char*)"audio/secret.wav");
sound *snd_03 = new sound((const char*)"audio/naruto.wav");

glm::mat4 projection;
glm::mat4 view;

int view_mat_location;
int proj_mat_location;

bool flagKey = false;
bool flagCastle = false;
bool debugP = false;
bool principalScreen = true;

int main(int argc, char **argv){

    init(g_gl_width, g_gl_height, &shader_programme);
    skybox *skyshok = new skybox(projection,view);
    gltInit();

    GLDebugDrawer *drawer = new GLDebugDrawer();
    world->getDynamicWorld()->setDebugDrawer(drawer);
    drawer->setView(&view);
    drawer->setProj(&projection);
    drawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);

    GLTtext *text = gltCreateText();
 //   gltSetText(text, "Consigue la llave!");
    int size = 3;
    int x = 10;
    int y = 10;

    while (!glfwWindowShouldClose(g_window)){
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gltColor(1.0f, 1.0f, 1.0f, 1.0f);
        gltDrawText2D(text,x, y, size);
	
        // input
        // -----
	   
        soundsPositioning();
        if(!principalScreen){
            input(g_window);
            if(!flagKey) gltSetText(text, "");

            // render
            // ------
            
            /*int numManifolds = world->getDynamicWorld()->getDispatcher()->getNumManifolds();
            for (int i = 0; i < numManifolds; i++)
            {
                btPersistentManifold* contactManifold =  world->getDynamicWorld()->getDispatcher()->getManifoldByIndexInternal(i);
                const btCollisionObject* obA = contactManifold->getBody0();
                const btCollisionObject* obB = contactManifold->getBody1();

                int numContacts = contactManifold->getNumContacts();
                //printf("%i\n",numContacts);
                for (int j = 0; j < numContacts; j++)
                {
                    btManifoldPoint& pt = contactManifold->getContactPoint(j);
                    if (pt.getDistance() < 0.f)
                    {
                        const btVector3& ptA = pt.getPositionWorldOnA();
                        const btVector3& ptB = pt.getPositionWorldOnB();
                        const btVector3& normalOnB = pt.m_normalWorldOnB;
                    }
                }
            }*/
            glUseProgram(shader_programme);
            if(!flagCastle && snd_01->get_source_state() != AL_PLAYING)
            {
                snd_01->play();
            }
            btTransform trans;
            if(sword->getRigidBody()->getLinearVelocity().length()<speedLimit){
                sword->getRigidBody()->applyImpulse(btVector3(impulso.x, 0.1f,impulso.z),btVector3(0,0,0));
                if(sword->getRigidBody()->getLinearVelocity().getY()<0.1f and sword->getRigidBody()->getLinearVelocity().getY()>-0.1f){
                    sword->getRigidBody()->applyImpulse(btVector3(0,jump.y,0),btVector3(0,0,0));
                }
            }
            //printf("%f\n",sword->getRigidBody()->getLinearVelocity().getY());
            world->stepSimulation();
            sword->getRigidBody()->getMotionState()->getWorldTransform(trans);
            posObj = glm::vec3(float(trans.getOrigin().getX()),float(trans.getOrigin().getY()),float(trans.getOrigin().getZ()));
            rotObj = sword->getRigidBody()->getOrientation();
            sword->transform(posObj,rotObj.getAngle(),rotObj.getAxis());

            btTransform trans2;
            cubo1->manager();
            cubo1->getRigidBody()->getMotionState()->getWorldTransform(trans2);
            cubo1->transform(glm::vec3(float(trans2.getOrigin().getX()),float(trans2.getOrigin().getY()),float(trans2.getOrigin().getZ())));

            cubo2->manager();
            cubo2->getRigidBody()->getMotionState()->getWorldTransform(trans2);
            cubo2->transform(glm::vec3(float(trans2.getOrigin().getX()),float(trans2.getOrigin().getY()),float(trans2.getOrigin().getZ())));

            

            update_camera();
            if(!flagKey &&( (cubo1->getPos().x > 17.2f && cubo1->getPos().z > -3.0f)
                        || (cubo2->getPos().x > 17.2f && cubo2->getPos().z > -3.0f))){
                flagKey = true;
                snd_02->play();
                gltSetText(text,"Pasa a la siguiente sala");
            }
            if(debugP){
                world->getDynamicWorld()->debugDrawWorld();
                drawer->drawLines();
            }else{
                sword->render(shader_programme);
                if(!flagCastle) {
                    terrenoExterior->render(shader_programme);
                    //espada->render(shader_programme);
                    castillo->render(shader_programme);
                   // arbolito->render(shader_programme);*/
                    skyshok->render(view);
                }else{
                    key->render(shader_programme);
                    piso->render(shader_programme);
                    piso2->render(shader_programme);
                    sala->render(shader_programme);
                    cubo1->render(shader_programme);
                    cubo2->render(shader_programme);
                }
            }
            if(!flagCastle && glfwGetKey(g_window, GLFW_KEY_P) == GLFW_PRESS){
                flagCastle=true;
                sword->getRigidBody()->translate(btVector3(-10.0f,-45.0f,15.0f)-trans.getOrigin());                
            }
        }else{
            if(glfwGetKey(g_window, GLFW_KEY_ENTER) == GLFW_PRESS) principalScreen=false;
            gltSetText(text,"PRESIONE ENTER PARA CONTINUAR");
        }
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    // Deleting text
    gltDeleteText(text);
    world->del();
    // Destroy glText
    gltTerminate();
    glfwTerminate();
    return 0;
}

