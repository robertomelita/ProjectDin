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
#include "cubo.h"

cubo::cubo(char* filename){
    this->pos = glm::vec3(0,0,0);
    this->model = glm::mat4();
    this->filename = filename;
    assert(this->load_mesh(filename, &(this->nvertices)));
    printf("Objeto %s cargado, %i vertices\n", filename, this->nvertices);
}
GLuint cubo::getVao(){
    return this->VAO;
}
GLuint cubo::getVbo(){
    return this->VBO;
}
glm::vec3 cubo::getPos(){
    return this->pos;
}
GLuint cubo::getTex(){
	return this->tex_rgb;
}
int cubo::getNvertices(){
    return this->nvertices;
}
void cubo::setPos(glm::vec3 pos){
	this->pos = pos;	
    this->model = glm::translate(glm::mat4(), this->pos);
}

void cubo::setMatloc(GLuint shaderprog, const char *name){	
	this->matloc = glGetUniformLocation (shaderprog, name);
}

void cubo::model2shader(GLuint shaderprog){
	// enviar matriz al shader (gpu)
	glUseProgram(shaderprog);
	glUniformMatrix4fv(this->matloc, 1, GL_FALSE, &(this->model[0][0]));
	GLfloat *position = (GLfloat*)malloc(sizeof(GLfloat)*3);
	position[0] = this->pos.x;
	position[1] = this->pos.y;
	position[2] = this->pos.z;	
	glUniform3fv(glGetUniformLocation(shaderprog,"pos_global"),3,&(position[0]));
}
bool cubo::load_texture (const char* file_name,GLuint *tex) {
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

void cubo::render(GLuint shader_programme){
	setMatloc(shader_programme,"model");
	glBindVertexArray(getVao());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,getTex());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,this->tex_normal);
	model2shader(shader_programme);
	glDrawArrays(GL_TRIANGLES,0,getNvertices());
}

void cubo::transform(glm::vec3 posObj){
	setPos(posObj);
}
void cubo::initPhysics(worldPhysics *world){	
	btCollisionShape* colShape = new btBoxShape(btVector3(2.48,2.48,2.48));
	world->getCollisionShapes().push_back(colShape);

	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(50.f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);

	startTransform.setOrigin(btVector3(this->pos.x, this->pos.y, this->pos.z));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	//rbInfo.m_friction = 50.0f;
	this->body = new btRigidBody(rbInfo);
	this->body->setActivationState(DISABLE_DEACTIVATION);
	this->body->setDamping(0.1f,0);
	this->body->setAngularFactor(0);
	world->addRigidBody(body);
}
btRigidBody* cubo::getRigidBody(){
	return this->body;
}
btRigidBody* cubo::capsule(){
	return this->cap;
}
bool cubo::load_texture_rgb(const char *filename, const char *sampler_name, GLuint* shaderprog){
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

bool cubo::load_texture_normal(const char *filename, const char *sampler_name, GLuint* shaderprog){
	glActiveTexture(GL_TEXTURE1);
	int code = load_texture(filename, &tex_normal);

	glUseProgram (*shaderprog);
    printf("getuniformlocation(%u, %s)\n", *shaderprog, sampler_name);
	texloc_normal = glGetUniformLocation( *shaderprog, sampler_name );
    printf("texloc_normal = %i\n", texloc_normal);
	assert( texloc_normal > -1 );
	glUniform1i( texloc_normal, 1 );
}
bool cubo::load_mesh (const char* file_name, int* point_count) {
	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate);
	if (!scene) {
		fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
		return false;
	}
	printf ("  %i animations\n", scene->mNumAnimations);
	printf ("  %i cameras\n", scene->mNumCameras);
	printf ("  %i lights\n", scene->mNumLights);
	printf ("  %i materials\n", scene->mNumMaterials);
	printf ("  %i meshes\n", scene->mNumMeshes);
	printf ("  %i textures\n", scene->mNumTextures);
	
	/* get first mesh in file only */
	const aiMesh* mesh = scene->mMeshes[0];
	printf ("    %i vertices in mesh[0]\n", mesh->mNumVertices);
	
	/* pass back number of vertex points in mesh */
	*point_count = mesh->mNumVertices;
	
	/* generate a VAO, using the pass-by-reference parameter that we give to the
	function */
	
	/* we really need to copy out all the data from AssImp's funny little data
	structures into pure contiguous arrays before we copy it into data buffers
	because assimp's texture coordinates are not really contiguous in memory.
	i allocate some dynamic memory to do this. */
	GLfloat* points = NULL; // array of vertex points
	GLfloat* normals = NULL; // array of vertex normals
	GLfloat* texcoords = NULL; // array of texture coordinates
    GLfloat* tangents = NULL;
	if (mesh->HasPositions ()) {
		points = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vp = &(mesh->mVertices[i]);
			points[i * 3] = (GLfloat)vp->x;
			points[i * 3 + 1] = (GLfloat)vp->y;
			points[i * 3 + 2] = (GLfloat)vp->z;
		}
	}
	if (mesh->HasNormals ()) {
		normals = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vn = &(mesh->mNormals[i]);
			normals[i * 3] = (GLfloat)vn->x;
			normals[i * 3 + 1] = (GLfloat)vn->y;
			normals[i * 3 + 2] = (GLfloat)vn->z;
		}
	}
	if (mesh->HasTextureCoords (0)) {
		texcoords = (GLfloat*)malloc (*point_count * 2 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
			texcoords[i * 2] = (GLfloat)vt->x;
			texcoords[i * 2 + 1] = (GLfloat)vt->y;
		}
	}
	if (mesh->HasTangentsAndBitangents ()) {
		tangents = (GLfloat *)malloc( *point_count * 4 * sizeof( GLfloat ) );
		for (int i = 0; i < *point_count; i++) {
				const aiVector3D *tangent = &( mesh->mTangents[i] );
				const aiVector3D *bitangent = &( mesh->mBitangents[i] );
				const aiVector3D *normal = &( mesh->mNormals[i] );

				// put the three vectors into my vec3 struct format for doing maths
				glm::vec3 t( tangent->x, tangent->y, tangent->z );
				glm::vec3 n( normal->x, normal->y, normal->z );
				glm::vec3 b( bitangent->x, bitangent->y, bitangent->z );
				// orthogonalise and normalise the tangent so we can use it in something
				// approximating a T,N,B inverse matrix
				glm::vec3 t_i = glm::normalize( t - n * glm::dot( n, t ) );

				// get determinant of T,B,N 3x3 matrix by dot*cross method
				float det = ( glm::dot( glm::cross( n, t ), b ) );
				if ( det < 0.0f ) {
					det = -1.0f;
				} else {
					det = 1.0f;
				}
				// push back 4d vector for inverse tangent with determinant
				tangents[i * 4 + 0] = t_i.x;
				tangents[i * 4 + 1] = t_i.y;
				tangents[i * 4 + 2] = t_i.z;
				tangents[i * 4 + 3] = det;

               
			    glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 0, NULL );
			    glEnableVertexAttribArray( 3 );
		}
	}
	this->makevao(points, normals, texcoords, tangents);

	free(points);
    free(normals);
    free(texcoords);
    free(tangents);

	aiReleaseImport (scene);
	printf ("mesh loaded\n");
	
	return true;
}
void cubo::makevao(GLfloat *vertices, GLfloat* normals, GLfloat *texcoords, GLfloat *tangents){
    glGenVertexArrays (1, &VAO);
    glBindVertexArray (VAO);
        if(vertices){
            glGenBuffers (1, &VBO);
            glBindBuffer (GL_ARRAY_BUFFER, VBO);
            glBufferData ( GL_ARRAY_BUFFER, 3 * nvertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray (0);
        }
        if(normals){
            glGenBuffers (1, &nbo);
            glBindBuffer (GL_ARRAY_BUFFER, nbo);
            glBufferData ( GL_ARRAY_BUFFER, 3 * nvertices * sizeof (GLfloat), normals, GL_STATIC_DRAW);
            glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray (1);
        }
        if(texcoords){
            glGenBuffers (1, &tbo);
            glBindBuffer (GL_ARRAY_BUFFER, tbo);
            glBufferData ( GL_ARRAY_BUFFER, 2 * nvertices * sizeof (GLfloat), texcoords, GL_STATIC_DRAW);
            glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray (2);
        }
		if(tangents){
			glGenBuffers( 1, &tanbo );
			glBindBuffer( GL_ARRAY_BUFFER, tanbo );
			glBufferData( GL_ARRAY_BUFFER, 4 * nvertices * sizeof( GLfloat ), tangents, GL_STATIC_DRAW );
			glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 0, NULL );
			glEnableVertexAttribArray( 3 );
		}
    glBindVertexArray(0);
}

void cubo::manager(){
    //printf();
    if(init && pos.y<=-47.3f/*this->body->getLinearVelocity().getY()==0*/){
		this->body->setLinearVelocity(btVector3(0,0,0));
        this->body->setLinearFactor(btVector3(1.0f,0,1.0f));
        init = false;
    }
	btVector3 velocity = body->getLinearVelocity();
	if(!init && !lockAxis && velocity.length()>0){
		if(abs(velocity.getX()) > abs(velocity.getZ())){
			if(velocity.getX()>0) this->body->setLinearVelocity(btVector3(5,0,0));
			else this->body->setLinearVelocity(btVector3(-5,0,0));
			this->body->setLinearFactor(btVector3(1,0,0));
		}else if(abs(velocity.getX()) < abs(velocity.getZ())){
			if(velocity.getZ()>0) this->body->setLinearVelocity(btVector3(0,0,5));
			else this->body->setLinearVelocity(btVector3(0,0,-5));
			this->body->setLinearFactor(btVector3(0,0,1));
		}
		lockAxis = true;
	}
	if(lockAxis && velocity.length()<0.5f){
		lockAxis=false;
		body->setLinearVelocity(btVector3(0,0,0));
		body->setLinearFactor(btVector3(1,0,1));
	}
}