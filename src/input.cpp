#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <math.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "input.h"

bool firstMouse = true;
float yaw   =  0.0f;
float pitch =  45.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;


void init_input(){
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(g_window, mouse_callback);
    glfwSetScrollCallback(g_window, scroll_callback);
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}



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

    if(pitch <=10.0f){
        pitch = 10.0f;
    }
    if(pitch >100.0f){
        pitch = 100.0f;
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
