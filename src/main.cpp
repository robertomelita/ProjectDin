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
#include "tools.h"
#include "malla.h"
#include "enemy.h"
#include "protagonist.h"
#include "suelo.h"

#define GL_LOG_FILE "log/gl.log"

int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow* g_window = NULL;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
int distancia = 12.0f;

glm::vec3 posObj = glm::vec3(0.0f,0,-2.0f);
glm::vec3 distanciaCamara = glm::vec3(0.0f, 1.0f, distancia);

// camera
glm::vec3 cameraPos   = posObj + glm::vec3(distancia, 0.5f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool raiznegativa = false;
bool firstMouse = true;
float yaw   =  0.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  -45.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;
float fov   =  45.0f;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLuint shader_programme;

int main(int argc, char **argv){

    init(g_gl_width, g_gl_height, &shader_programme);

    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(g_window, mouse_callback);
    glfwSetScrollCallback(g_window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /*-------------------------------CREATE SHADERS-------------------------------*/

    suelo *sword = new suelo((char*)"mallas/cosa.obj");
    sword->setPos(posObj);
    sword->setMatloc(shader_programme,"model");

    suelo *piso = new suelo((char*)"mallas/suelo.obj");
    piso->setPos(glm::vec3(0,-5.0f,0));
    piso->setMatloc(shader_programme,"model");
    

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(cameraPos, posObj, cameraUp);

		
    int view_mat_location = glGetUniformLocation (shader_programme, "view");
    glUseProgram (shader_programme);
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
    int proj_mat_location = glGetUniformLocation (shader_programme, "proj");
    glUseProgram (shader_programme);
    glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);
    

    /*for(int i = 0; i<N; i++){
        glm::vec3 mono1 = monos[i]->getpos();
        for(int j = 0; j<N ; j++){
            glm::vec3 mono2 = monos[j]->getpos();
            if (mono1.x <= mono2.x+1.0f && mono1.x >= mono2.x-1.0f && mono1.y <= mono2.y+1.0f && mono1.y >= mono2.y-1.0f && mono1.z <= mono2.z+1.0f && mono1.z >= mono2.z-1.0f && i!=j)printf("mono %i, colisiona con mono %i\n",i,j);
        }
    }*/
	

    // render loop
    // -----------
    while (!glfwWindowShouldClose(g_window)){
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(g_window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
	    glUseProgram (shader_programme);

        // pass projection matrix to shader (note that in this case it could change every frame)
        projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
        glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, &projection[0][0]);

        // camera/view transformation
        view = glm::lookAt(cameraPos, posObj, cameraUp);
	    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);

	    // dibujar los N monos
	
       /* for(int i=0; i<N; ++i){
            glBindVertexArray(monos[i]->getvao());
            // copiar matriz model de cada mono, antes de dibujarlo
            monos[i]->model2shader(shader_programme);
            glDrawArrays(GL_TRIANGLES,0,monos[i]->getnumvertices());
        }
        glBindVertexArray(cosa->getVao());
        cosa->model2shader(shader_programme);
        glDrawArrays(GL_TRIANGLES,0,cosa->getNvertices());*/

        sword->setPos(posObj);
        sword->setMatloc(shader_programme,"model");
        glBindVertexArray(sword->getVao());
        sword->model2shader(shader_programme);
        glDrawArrays(GL_TRIANGLES,0,sword->getNvertices());

        glBindVertexArray(piso->getVao());
        piso->model2shader(shader_programme);
        glDrawArrays(GL_TRIANGLES,0,piso->getNvertices());


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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        posObj += glm::vec3(0,0,-1.0f)*cameraSpeed;
        cameraPos += glm::vec3(0,0,-1.0f)*cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        posObj += glm::vec3(0,0,1.0f)*cameraSpeed;
        cameraPos += glm::vec3(0,0,1.0f)*cameraSpeed;
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        posObj += glm::vec3(-1.0f,0,0)*cameraSpeed;
        cameraPos += glm::vec3(-1.0f,0,0)*cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        posObj += glm::vec3(1.0f,0,0)*cameraSpeed;
        cameraPos += glm::vec3(1.0f,0,0)*cameraSpeed;
    }
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    
    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    pitch += yoffset;
    yaw   += xoffset;

    if(pitch >=-10.0f){
        pitch = -10.0f;
    }
    if(pitch <-100.0f){
        pitch = -100.0f;
    }

    cameraPos.x = (distancia*sin(glm::radians(pitch))*cos(glm::radians(yaw)))+posObj.x;
    cameraPos.z = (distancia*sin(glm::radians(pitch))*sin(glm::radians(yaw)))+posObj.z;
    cameraPos.y = (distancia*cos(glm::radians(pitch)))+posObj.y;
   
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
