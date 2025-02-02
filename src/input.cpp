#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <math.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "input.h"

bool firstMouse = true;
float yaw   =  90.0f;
float pitch =  90.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;


void init_input(){
    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(g_window, mouse_callback);
    glfwSetScrollCallback(g_window, scroll_callback);
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void soundsPositioning()
{
    snd_01->set_listener(cameraPos);
    snd_02->set_listener(cameraPos);
    maintheme->set_listener(cameraPos);
    maz->set_listener(cameraPos);
}


void input(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    impulso = glm::vec3(0,0,0);
    jump = glm::vec3(0,0,0);
    bool wPressed=false;
    bool sPressed=false;
    float vectorTranquibarranqui = 10.0f;
    speedLimit = 8.0f;
    if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)){
        speedLimit = 20.0f;
    }
    //float vectorTranquibarranqui = 10.f;
    glm::vec3 vectorDirector = glm::normalize(posObj-cameraPos);
    glm::vec3 vectorPerpendicular = glm::normalize(glm::cross(vectorDirector,glm::vec3(0,1.0f,0)));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
    //    posObj += vectorDirector*cameraSpeed;
    //    cameraPos += vectorDirector*cameraSpeed;
    //    traslacion += vectorDirector*vectorTranquibarranqui;.
        impulso = glm::normalize((vectorDirector-glm::vec3(0,vectorDirector.y,0)))*vectorTranquibarranqui;
        yawPersonaje=yaw;
        wPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
    //    posObj -= vectorDirector*cameraSpeed;
    //    cameraPos -= vectorDirector*cameraSpeed;
    //    traslacion -= vectorDirector*vectorTranquibarranqui;
        impulso +=-glm::normalize(vectorDirector-glm::vec3(0,vectorDirector.y,0))*vectorTranquibarranqui;
        yawPersonaje=yaw-180.0f;
        sPressed = true;
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
    //    posObj -= vectorPerpendicular*cameraSpeed;
    //    cameraPos -= vectorPerpendicular*cameraSpeed;
    //    traslacion -= vectorPerpendicular*vectorTranquibarranqui;
        impulso +=-glm::normalize(vectorPerpendicular)*vectorTranquibarranqui;
        if(wPressed) yawPersonaje=yaw-45.0f;
        else if(sPressed) yawPersonaje=yaw-125.0f;
        else yawPersonaje=yaw-90.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
    //    posObj += vectorPerpendicular*cameraSpeed;
    //    cameraPos += vectorPerpendicular*cameraSpeed;
    //    traslacion += vectorPerpendicular*vectorTranquibarranqui;
        impulso +=glm::normalize(vectorPerpendicular)*vectorTranquibarranqui;
        if(wPressed) yawPersonaje=yaw+45.0f;
        else if(sPressed) yawPersonaje=yaw+125.0f;
        else yawPersonaje=yaw+90.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        jump = glm::vec3(0,500.0f,0);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        impulso += glm::vec3(0,-10.0f,0);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        debugP= !debugP;
    }

    cameraPos.x = (distancia*sin(glm::radians(pitch))*cos(glm::radians(yaw)))+posObj.x;
    cameraPos.z = (distancia*sin(glm::radians(pitch))*sin(glm::radians(yaw)))+posObj.z;
    cameraPos.y = (distancia*cos(glm::radians(pitch)))+posObj.y;


/*
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
         if(snd_02->get_source_state() != AL_PLAYING)
        {
            snd_02->play();
        }
    }*/
    
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
    if(pitch >150.0f){
        pitch = 150.0f;
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
void update_camera(){
    // pass projection matrix to shader (note that in this case it could change every frame)
    projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 10001.0f);
    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, &projection[0][0]);

    // camera/view transformation
    view = glm::lookAt(cameraPos, posObj, cameraUp);
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
}
