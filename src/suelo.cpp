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
#include "suelo.h"

suelo::suelo(char* filename){
    this->pos = glm::vec3(0,0,0);
    this->model = glm::mat4();
    this->filename = filename;
    assert(load_mesh(filename, &(this->VAO), &(this->nvertices)));
    printf("Objeto %s cargado, %i vertices\n", filename, this->nvertices);
}
GLuint suelo::getVao(){
    return this->VAO;
}
GLuint suelo::getVbo(){
    return this->VBO;
}
glm::vec3 suelo::getPos(){
    return this->pos;
}
int suelo::getNvertices(){
    return this->nvertices;
}
void suelo::setPos(glm::vec3 pos){
	this->pos = pos;	
    this->model = glm::translate(glm::mat4(), this->pos);
}

void suelo::setMatloc(GLuint shaderprog, const char *name){	
	this->matloc = glGetUniformLocation (shaderprog, name);
}

void suelo::model2shader(GLuint shaderprog){
	// enviar matriz al shader (gpu)
	glUseProgram(shaderprog);
	glUniformMatrix4fv(this->matloc, 1, GL_FALSE, &(this->model[0][0]));
}