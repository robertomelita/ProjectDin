#ifndef INPUT_H
#define INPUT_H

extern int g_gl_width;
extern int g_gl_height;
extern GLFWwindow* g_window;
extern float fov;

extern int distancia;

extern glm::vec3 posObj;

// camera
extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

extern float deltaTime;	// time between current frame and last frame
extern float lastFrame;

void init_input();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

#endif