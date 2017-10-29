#ifndef INPUT_H
#define INPUT_H
#include "sound.h"
extern int g_gl_width;
extern int g_gl_height;
extern GLFWwindow* g_window;
extern float fov;

extern int distancia;

extern glm::vec3 posObj;
extern GLuint shader_programme;
// camera
extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

extern float deltaTime;	// time between current frame and last frame
extern float lastFrame;

extern glm::mat4 projection;
extern glm::mat4 view;

extern int view_mat_location;
extern int proj_mat_location;

extern sound *snd_01;
extern sound *snd_02;
//sound
//extern sound *snd_01;

void init_input();
void soundsPositioning();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void input(GLFWwindow *window);
void update_camera();

#endif