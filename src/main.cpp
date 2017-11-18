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
glm::vec3 posObj = glm::vec3(0.0f,0,-2.0f);

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
suelo *castillo;
suelo *espada;
suelo *arbolito;
suelo *esfera;
worldPhysics* world;
enemy *key;
sound *snd_01 = new sound((const char*)"audio/rito2.wav");
sound *snd_02 = new sound((const char*)"audio/secret.wav");
sound *snd_03 = new sound((const char*)"audio/naruto.wav");

glm::mat4 projection;
glm::mat4 view;

int view_mat_location;
int proj_mat_location;

bool flagKey = false;
bool flagCastle = false;

int main(int argc, char **argv){

    init(g_gl_width, g_gl_height, &shader_programme);
    skybox *skyshok = new skybox(projection,view);
    gltInit();

   /* GLDebugDrawer *drawer = new GLDebugDrawer();
    world->getDynamicWorld()->setDebugDrawer(drawer);
    drawer->setView(&view);
    drawer->setProj(&projection);
    drawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);*/

    GLTtext *text = gltCreateText();
    gltSetText(text, "Consigue la llave!");
    int size = 3;
    int x = 10;
    int y = 10;

    while (!glfwWindowShouldClose(g_window)){
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gltColor(1.0f, 1.0f, 1.0f, 0.0f);
        gltDrawText2D(text,x, y, size);
	
        // input
        // -----
	   
        input(g_window);
        soundsPositioning();


        // render
        // ------
        world->stepSimulation();
/*        world->getDynamicWorld()->debugDrawWorld();
        drawer->drawLines();*/
        int numManifolds = world->getDynamicWorld()->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold =  world->getDynamicWorld()->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = contactManifold->getBody0();
            const btCollisionObject* obB = contactManifold->getBody1();

            int numContacts = contactManifold->getNumContacts();
            printf("%i\n",numContacts);
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
        }
        
        glUseProgram(shader_programme);
        update_camera();
        if(!flagCastle && snd_01->get_source_state() != AL_PLAYING)
        {
            snd_01->play();
        }
        btTransform trans;
        sword->getRigidBody()->getMotionState()->getWorldTransform(trans);
        if(/*impulso.length()!=0 && */sword->getRigidBody()->getLinearVelocity().length()<speedLimit) sword->getRigidBody()->applyImpulse(btVector3(impulso.x,impulso.y,impulso.z),btVector3(0,0,0));
        sword->getRigidBody()->setAngularFactor(0);
        posObj = glm::vec3(float(trans.getOrigin().getX()),float(trans.getOrigin().getY()),float(trans.getOrigin().getZ()));
        sword->transform(posObj,yawPersonaje);
        sword->render(shader_programme);
        if(!flagKey && posObj.x > 10.0f && posObj.x < 20.0f && posObj.z > -101.0f && posObj.z <-99.0f ){
            flagKey = true;
            snd_02->play();
            gltSetText(text,"Ahora entra al castillo");
        }
        if(flagKey && !flagCastle && posObj.x < -18.0f && posObj.x > -24.0f && posObj.z > -135.0f && posObj.z <-134.0f){
            flagCastle = true;
            gltSetText(text,"GANASTE!!!!");
            snd_01->stop();
            snd_03->play();
            size = 8;
            x = 20;
            y = (g_gl_height/2)-20;
        }
        if(!flagKey) key->render(shader_programme);
        piso->render(shader_programme);
        espada->render(shader_programme);
        castillo->render(shader_programme);
        arbolito->render(shader_programme);
        esfera->render(shader_programme);
        skyshok->render(view);
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

