#ifndef TOOLS_H
#define TOOLS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl_utils.h"
#include "tools.h"
#include "malla.h"
#include "enemy.h"
#include "protagonist.h"
#include "suelo.h"
#include "input.h"
#include "sound.h"

using namespace std;

extern GLuint shader_programme;

//mallas
extern suelo *sword;
extern suelo *piso;
extern suelo *castillo;
extern suelo *espada;

extern glm::mat4 projection;
extern glm::mat4 view;

extern int view_mat_location;
extern int proj_mat_location;

extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

extern int distancia;

extern glm::vec3 posObj;


// funcion que carga una malla desde filename
bool load_mesh (const char* file_name, GLuint* vao, int* point_count);
// funcion que inicializa openGL
void init(int g_gl_width, int g_gl_height, GLuint *shader_programme);
#endif
