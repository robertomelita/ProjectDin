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
#include "malla.h"

using namespace std;
malla::malla(char* filename){
    this->pos = glm::vec3(0,0,0);
    this->model = glm::mat4();
    this->filename = filename;
    assert(load_mesh(filename, &(this->vao), &(this->numvertices)));
    printf("Malla %s cargada, %i vertices\n", filename, this->numvertices);
}

GLuint malla::getvao(){
    return this->vao;
}

int malla::getnumvertices(){
    return this->numvertices;
}

glm::vec3 malla::getpos(){
    return this->pos;
}

int malla::getmatloc(){
    return this->matloc;
}

char* malla::getfilename(){
    return this->filename;
}

void malla::setpos(glm::vec3 pos){
	this->pos = pos;	
	this->model = glm::translate(glm::mat4(), this->pos);
}

void malla::setmatloc(GLuint shaderprog, const char *name){	
	this->matloc = glGetUniformLocation (shaderprog, name);
}

void malla::model2shader(GLuint shaderprog){
	// enviar matriz al shader (gpu)
	glUseProgram(shaderprog);
	glUniformMatrix4fv(this->matloc, 1, GL_FALSE, &(this->model[0][0]));
}

void malla::printmodel(){
	for(int i=0; i<4; ++i){
		for(int j=0; j<4; ++j){
			printf("%f ", this->model[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}



































