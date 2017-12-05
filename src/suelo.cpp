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

suelo::suelo(char* filename,float roce){
    this->pos = glm::vec3(0,0,0);
    this->model = glm::mat4();
	this->roce = roce;
    this->filename = filename;
    assert(this->load_mesh(filename, &(this->nvertices)));
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
void suelo::transform(glm::vec3 posObj, float rot, btVector3 rotVec){
	setPos(posObj);
	sword->setRot(rot,glm::vec3(rotVec.getX(),rotVec.getY(),rotVec.getZ()));
}
void suelo::setRot(float angle){
    this->model = glm::rotate(this->model,(glm::mediump_float)glm::radians(angle),glm::vec3(0,1.0f,0));
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
void suelo::render(GLuint shader_programme){
	setMatloc(shader_programme,"model");
	glBindVertexArray(getVao());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,getTex());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,this->tex_normal);
	model2shader(shader_programme);
	glDrawArrays(GL_TRIANGLES,0,getNvertices());
	glBindVertexArray(0);
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
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 16.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	return true;
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
	rbInfo.m_friction = this->roce;
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
	glUseProgram(0);
}

bool suelo::load_mesh (const char* file_name, int* point_count) {
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
void suelo::makevao(GLfloat *vertices, GLfloat* normals, GLfloat *texcoords, GLfloat *tangents){
    glGenVertexArrays (1, &VAO);
    glBindVertexArray (VAO);
        if(vertices){
			glEnableVertexAttribArray (0);
            glGenBuffers (1, &VBO);
            glBindBuffer (GL_ARRAY_BUFFER, VBO);
            glBufferData ( GL_ARRAY_BUFFER, 3 * nvertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            
        }
        if(normals){
			glEnableVertexAttribArray (1);
            glGenBuffers (1, &nbo);
            glBindBuffer (GL_ARRAY_BUFFER, nbo);
            glBufferData ( GL_ARRAY_BUFFER, 3 * nvertices * sizeof (GLfloat), normals, GL_STATIC_DRAW);
            glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            
        }
        if(texcoords){
			glEnableVertexAttribArray (2);
            glGenBuffers (1, &tbo);
            glBindBuffer (GL_ARRAY_BUFFER, tbo);
            glBufferData ( GL_ARRAY_BUFFER, 2 * nvertices * sizeof (GLfloat), texcoords, GL_STATIC_DRAW);
            glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray (2);
        }
		if(tangents){
			glEnableVertexAttribArray (3);
			glGenBuffers( 1, &tanbo );
			glBindBuffer( GL_ARRAY_BUFFER, tanbo );
			glBufferData( GL_ARRAY_BUFFER, 4 * nvertices * sizeof( GLfloat ), tangents, GL_STATIC_DRAW );
			glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 0, NULL );
			glEnableVertexAttribArray( 3 );
		}
    glBindVertexArray(0);
}