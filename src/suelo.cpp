#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
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
#include "stb_image.h"
#include "suelo.h"

using namespace std;

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
GLuint suelo::getTex(){
	return this->tex_rgb;
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
	glUniform1f(glGetUniformLocation(shaderprog,"ConstA"),this->ConstA);
	glUniform1f(glGetUniformLocation(shaderprog,"ConstD"),this->ConstD);
	glUniform1f(glGetUniformLocation(shaderprog,"ConstS"),this->ConstS);
}

bool suelo::load_texture (const char* file_name, GLuint *tex) {
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
	glGenTextures (1, tex);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, *tex);
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

void suelo::render(GLuint shader_programme){
	setMatloc(shader_programme,"model");
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(getVao());
	glBindTexture(GL_TEXTURE_2D,getTex());
	model2shader(shader_programme);
	glDrawArrays(GL_TRIANGLES,0,getNvertices());
}

void suelo::transform(glm::vec3 posObj){
	setPos(posObj);
}
void suelo::initPhysics(worldPhysics *world){
	btTriangleMesh* originalMesh = new btTriangleMesh();
		const aiScene* scene = aiImportFile(this->filename, aiProcess_Triangulate);
		if (!scene) {
			fprintf (stderr, "ERROR: reading mesh\n");
			return;
		}
			
		const aiMesh* mesh = scene->mMeshes[0];
		printf ("    %i vertices in mesh[0]\n", mesh->mNumVertices);
			
		if (mesh->HasPositions ()) {
			for (int i = 0; i < mesh->mNumVertices/3; i++) {
				const aiVector3D* vp1 = &(mesh->mVertices[3*i]);
				const aiVector3D* vp2 = &(mesh->mVertices[3*i+1]);
				const aiVector3D* vp3 = &(mesh->mVertices[3*i+2]);
				originalMesh->addTriangle(btVector3(vp1->x,vp1->y,vp1->z),btVector3(vp2->x,vp2->y,vp2->z) ,btVector3(vp3->x,vp3->y,vp3->z),false);
			}
		}

	btCollisionShape* groundShape = new btBvhTriangleMeshShape(originalMesh ,true);//btStaticPlaneShape(btVector3(0, 1, 0),1);

	world->getCollisionShapes().push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(this->pos.x,this->pos.y,this->pos.z));

	btScalar mass(0.);

	bool isDynamic = (mass != 0.f);
	
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		groundShape->calculateLocalInertia(mass, localInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	rbInfo.m_friction = 2.0f;
	this->body = new btRigidBody(rbInfo);
	//add the body to the dynamics world
	world->addRigidBody(this->body);
}

btRigidBody* suelo::getRigidBody(){
	return this->body;
}
void suelo::setLightConstants(GLfloat ConstA,GLfloat ConstD,GLfloat ConstS){
	this->ConstA=ConstA;
	this->ConstD=ConstD;
	this->ConstS=ConstS;
}

bool suelo::load_texture_rgb(const char *filename, const char *sampler_name, GLuint* shaderprog){
	glActiveTexture(GL_TEXTURE0);
	int code = load_texture(filename, &tex_rgb);

	glUseProgram (*shaderprog);
    printf("getuniformlocation(%u, %s)\n", *shaderprog, sampler_name);
	texloc_rgb = glGetUniformLocation( *shaderprog, sampler_name );
    printf("texloc_rgb = %i\n", texloc_rgb);
	assert( texloc_rgb > -1 );
	glUniform1i( texloc_rgb, 0);
    glUseProgram(0);
}

bool suelo::load_texture_normal(const char *filename, const char *sampler_name, GLuint* shaderprog){
	glActiveTexture(GL_TEXTURE1);
	int code = load_texture(filename, &tex_normal);

	glUseProgram (*shaderprog);
    printf("getuniformlocation(%u, %s)\n", *shaderprog, sampler_name);
	texloc_normal = glGetUniformLocation( *shaderprog, sampler_name );
    printf("texloc_normal = %i\n", texloc_normal);
	assert( texloc_normal > -1 );
	glUniform1i( texloc_normal, 1 );
}
