#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdlib.h>
#include <btBulletDynamicsCommon.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "worldPhysics.h"
#include "stb_image.h"
#include "tools.h"
#include "protagonist.h"

protagonist::protagonist(char* filename){
    this->pos = glm::vec3(0,0,0);
    this->model = glm::mat4();
    this->filename = filename;
    assert(load_mesh(filename, &(this->VAO), &(this->nvertices)));
    printf("Objeto %s cargado, %i vertices\n", filename, this->nvertices);
}
GLuint protagonist::getVao(){
    return this->VAO;
}
GLuint protagonist::getVbo(){
    return this->VBO;
}
int protagonist::getVida(){
    return this->vida;
}
int protagonist::getPoder(){
    return this->poder;
}
glm::vec3 protagonist::getPos(){
    return this->pos;
}
GLuint protagonist::getTex(){
	return this->tex;
}
int protagonist::getNvertices(){
    return this->nvertices;
}
void protagonist::setPos(glm::vec3 pos){
	this->pos = pos;	
    this->model = glm::translate(glm::mat4(), this->pos);
}

void protagonist::setMatloc(GLuint shaderprog, const char *name){	
	this->matloc = glGetUniformLocation (shaderprog, name);
}

void protagonist::model2shader(GLuint shaderprog){
	// enviar matriz al shader (gpu)
	glUseProgram(shaderprog);
	glUniformMatrix4fv(this->matloc, 1, GL_FALSE, &(this->model[0][0]));
}
bool protagonist::load_texture (const char* file_name) {
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

void protagonist::render(GLuint shader_programme){
	setMatloc(shader_programme,"model");
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(getVao());
	glBindTexture(GL_TEXTURE_2D,getTex());
	model2shader(shader_programme);
	glDrawArrays(GL_TRIANGLES,0,getNvertices());
}

void protagonist::transform(glm::vec3 posObj){
	setPos(posObj);
}
void protagonist::initPhysics(worldPhysics *world){
	btCollisionShape* colShape = new btCapsuleShape(.5f,5);/*btConvexTriangleMeshShape(originalMesh,true);*/  //btSphereShape(btScalar(1.f));
	world->getCollisionShapes().push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(100.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(posObj.x, posObj.y, posObj.z));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setActivationState(DISABLE_DEACTIVATION);
	body->setDamping(0.1f,0.1f);
	world->addRigidBody(body);
}