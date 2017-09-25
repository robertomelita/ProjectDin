#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tools.h"
#include "enemy.h"

enemy::enemy(char* filename){
    this->pos = glm::vec3(0,0,0);
    this->model = glm::mat4();
    this->filename = filename;
    assert(load_mesh(filename, &(this->VAO), &(this->nvertices)));
    printf("Malla %s cargada, %i vertices\n", filename, this->nvertices);
}

GLuint enemy::getVao(){
    return this->VAO;
}
GLuint enemy::getVbo(){
    return this->VBO;
}
int enemy::getVida(){
    return this->vida;
}
int enemy::getPoder(){
    return this->poder;
}
glm::vec3 enemy::getPos(){
    return this->pos;
}
int enemy::getNvertices(){
    return this->nvertices;
}
void enemy::setPos(glm::vec3 pos){
	this->pos = pos;	
	this->model = glm::translate(glm::mat4(), this->pos);
}

void enemy::setMatloc(GLuint shaderprog, const char *name){	
	this->matloc = glGetUniformLocation (shaderprog, name);
}

void enemy::model2shader(GLuint shaderprog){
	// enviar matriz al shader (gpu)
	glUseProgram(shaderprog);
	glUniformMatrix4fv(this->matloc, 1, GL_FALSE, &(this->model[0][0]));
}