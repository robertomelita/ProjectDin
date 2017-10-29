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

#define GL_LOG_FILE "log/gl.log"
using namespace std;

int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow* g_window = NULL;

int distancia = 12.0f;

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
suelo *sword;
suelo *piso;
suelo *castillo;
suelo *espada;

glm::mat4 projection;
glm::mat4 view;

int view_mat_location;
int proj_mat_location;

int main(int argc, char **argv){

    init(g_gl_width, g_gl_height, &shader_programme);

    while (!glfwWindowShouldClose(g_window)){
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        input(g_window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        glUseProgram(shader_programme);
        
        update_camera();

        sword->transform(posObj);
        sword->render(shader_programme);
   
        piso->render(shader_programme);
        castillo->render(shader_programme);
        espada->render(shader_programme);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
