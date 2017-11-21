#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
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
bool enemy::load_texture (const char* file_name) {
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load (file_name, &x, &y, &n, force_channels);
    //printf("x = %i    y = %i\n", x, y);
	if (!image_data) {
		fprintf (stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf (
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
		);
	}
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++) {
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	glGenTextures (1, &tex);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, tex);
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap (GL_TEXTURE_2D);
    // probar cambiar GL_CLAMP_TO_EDGE por GL_REPEAT
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 16.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	return true;
}

void enemy::render(GLuint shader_programme){
	setMatloc(shader_programme,"model");
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(getVao());
	glBindTexture(GL_TEXTURE_2D,this->tex);
	model2shader(shader_programme);
	glDrawArrays(GL_TRIANGLES,0,getNvertices());
	glBindVertexArray(0);
}

void enemy::transform(glm::vec3 posObj){
	setPos(posObj);
}
