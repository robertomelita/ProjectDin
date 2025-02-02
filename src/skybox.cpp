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
#include "stb_image.h"
#include "gl_utils.h"
#include "skybox.h"

#define CUBE_VERT_FILE "shaders/cube_vs.glsl"
#define CUBE_FRAG_FILE "shaders/cube_fs.glsl"
#define FRONT "textures/Box_Back.bmp"
#define BACK "textures/Box_Front.bmp"
#define TOP "textures/Box_Top.bmp"
#define BOTTOM "textures/Box_Bottom.bmp"
#define LEFT "textures/Box_Left.bmp"
#define RIGHT "textures/Box_Right.bmp"

skybox::skybox(glm::mat4 projection,glm::mat4 view){
    float points[] = {
		-500.0f,  500.0f, -500.0f,
		-500.0f, -500.0f, -500.0f,
		 500.0f, -500.0f, -500.0f,
		 500.0f, -500.0f, -500.0f,
		 500.0f,  500.0f, -500.0f,
		-500.0f,  500.0f, -500.0f,

		-500.0f, -500.0f,  500.0f,
		-500.0f, -500.0f, -500.0f,
		-500.0f,  500.0f, -500.0f,
		-500.0f,  500.0f, -500.0f,
		-500.0f,  500.0f,  500.0f,
		-500.0f, -500.0f,  500.0f,

		 500.0f, -500.0f, -500.0f,
		 500.0f, -500.0f,  500.0f,
		 500.0f,  500.0f,  500.0f,
		 500.0f,  500.0f,  500.0f,
		 500.0f,  500.0f, -500.0f,
		 500.0f, -500.0f, -500.0f,

		-500.0f, -500.0f,  500.0f,
		-500.0f,  500.0f,  500.0f,
		 500.0f,  500.0f,  500.0f,
		 500.0f,  500.0f,  500.0f,
		 500.0f, -500.0f,  500.0f,
		-500.0f, -500.0f,  500.0f,

		-500.0f,  500.0f, -500.0f,
		 500.0f,  500.0f, -500.0f,
		 500.0f,  500.0f,  500.0f,
		 500.0f,  500.0f,  500.0f,
		-500.0f,  500.0f,  500.0f,
		-500.0f,  500.0f, -500.0f,

		-500.0f, -500.0f, -500.0f,
		-500.0f, -500.0f,  500.0f,
		 500.0f, -500.0f, -500.0f,
		 500.0f, -500.0f, -500.0f,
		-500.0f, -500.0f,  500.0f,
		 500.0f, -500.0f,  500.0f
	};
	glGenBuffers (1, &this->vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, 3 * 36 * sizeof (GLfloat), &points, GL_STATIC_DRAW);

	glGenVertexArrays (1, &this->vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    createCubeMap(FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT);

    this->cube_sp= create_programme_from_files(CUBE_VERT_FILE, CUBE_FRAG_FILE);
    this->cube_V_location = glGetUniformLocation (cube_sp, "V");
    this->cube_P_location = glGetUniformLocation (cube_sp, "P");
    
    glUseProgram (this->cube_sp);
	glUniformMatrix4fv (cube_V_location, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv (cube_P_location, 1, GL_FALSE, &projection[0][0]);
}


bool skybox::loadCubeMap (GLenum side_target, const char* file_name) {
	glBindTexture (GL_TEXTURE_CUBE_MAP, this->cube_texture);

	int x, y, n;
	int force_channels = 4;
	unsigned char*  image_data = stbi_load (file_name, &x, &y, &n, force_channels);
	if (!image_data) {
        fprintf (stderr, "ERROR: could not load %s\n", file_name);
        return false;
	}
	// non-power-of-2 dimensions check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf (stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name);
	}

	// copy image data into 'target' side of cube map
	glTexImage2D (side_target,0,GL_RGBA,x,y,0,GL_RGBA,GL_UNSIGNED_BYTE,image_data);
    free (image_data);
    return true;
}
void skybox::createCubeMap(	const char* front,const char* back,const char* top,const char* bottom,const char* left,const char* right) {
	// generate a cube-map texture to hold all the sides
	glActiveTexture (GL_TEXTURE0);
	glGenTextures (1, &this->cube_texture);

	// load each image and copy into a side of the cube-map texture
	assert (loadCubeMap (GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front));
	assert (loadCubeMap (GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back));
	assert (loadCubeMap (GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top));
	assert (loadCubeMap (GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom));
	assert (loadCubeMap (GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left));
	assert (loadCubeMap (GL_TEXTURE_CUBE_MAP_POSITIVE_X, right));
	// format cube map texture
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
void skybox::render(glm::mat4 view){
    glDepthMask (GL_FALSE);
    glUseProgram (this->cube_sp);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_CUBE_MAP, this->cube_texture);
    glBindVertexArray (this->vao);
    glDrawArrays (GL_TRIANGLES, 0, 36);
    glDepthMask (GL_TRUE);
    glUseProgram (this->cube_sp);
    glUniformMatrix4fv (this->cube_V_location, 1, GL_FALSE,&view[0][0]);
}