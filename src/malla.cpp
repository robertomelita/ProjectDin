#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include "maths_funcs.h"
#include "gl_utils.h"
#include "tools.h"
#include "stb_image.h"
#include "malla.h"

using namespace std;
malla::malla(const char* filename, GLuint shader_program, GLuint face_type){
	vec3 pos = vec3(0,0,0);
    rx = ry = rz = 0.0;
    set_shaderprog(shader_program);
	glUseProgram (this->shaderprog);
	set_matloc(glGetUniformLocation (this->shaderprog, "model"));

	this->filename = (char*)filename;
	assert(load_mesh(filename, face_type));
    reset_matrix();
}

malla::malla(){
	vec3 pos = vec3(0,0,0);
}

// functions
void malla::reset_matrix(){
    this->M = identity_mat4();
}

void malla::print_matrix(){
    print(this->M);
}

void malla::mtranslate(vec3 d){
    this->M = translate(get_matrix(), d); 
}

// accumulative
void malla::mrotatey(float d){
    ry += d;
    this->M = rotate_y_deg(get_matrix(), ry); 
}

void malla::render_vertices(){
    glBindVertexArray(this->vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_rgb);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,this->tex_normal);
    glUniformMatrix4fv(get_matloc(), 1, GL_FALSE, get_matrix().m);
    glDrawArrays(get_facetype(), 0, get_numvertices());

    glBindVertexArray(0);
}

void malla::render_vertices_points(GLfloat point_size){
    glActiveTexture(GL_TEXTURE0);
    glPointSize(point_size);
    glBindVertexArray(this->vao);
    glBindTexture(GL_TEXTURE_2D, tex_rgb);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,this->tanbo);
	glUniformMatrix4fv(get_matloc(), 1, GL_FALSE, get_matrix().m);
    glDrawArrays(GL_POINTS, 0, get_numvertices());
    glPointSize(1.0f);
    glBindVertexArray(0);
}

void malla::render_indices(){
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->vao);
    glBindTexture(GL_TEXTURE_2D, tex_rgb);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,this->tanbo);
	glUniformMatrix4fv(get_matloc(), 1, GL_FALSE, get_matrix().m);
    glDrawElements(get_facetype(), get_facefactor()*get_numfaces(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void malla::makevao(GLfloat *vertices, GLfloat* normals, GLfloat *texcoords, GLfloat *tangents, GLuint *indices){
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
        if(vertices){
			glEnableVertexAttribArray (0);
            glGenBuffers (1, &vbo);
            glBindBuffer (GL_ARRAY_BUFFER, vbo);
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
            
        }
		if(tangents){
			glEnableVertexAttribArray(3);
			glGenBuffers( 1, &tanbo );
			glBindBuffer( GL_ARRAY_BUFFER, tanbo );
			glBufferData( GL_ARRAY_BUFFER, 4 * nvertices * sizeof( GLfloat ), tangents, GL_STATIC_DRAW );
			glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		}
    glBindVertexArray(0);
}

bool malla::load_mesh (const char* file_name, GLuint face_type) {
	// triangular malla y calcular tangent space
	const aiScene *scene = aiImportFile( file_name, aiProcess_Triangulate | aiProcess_CalcTangentSpace );
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
	this->set_numvertices(mesh->mNumVertices);
	this->set_numfaces(mesh->mNumFaces);
    this->set_facetype(face_type);
    this->set_facefactor(0);
	
	/* we really need to copy out all the data from AssImp's funny little data
	structures into pure contiguous arrays before we copy it into data buffers
	because assimp's texture coordinates are not really contiguous in memory.
	i allocate some dynamic memory to do this. */
	GLfloat* points = NULL; // array of vertex points
	GLfloat* normals = NULL; // array of vertex normals
	GLfloat* texcoords = NULL; // array of texture coordinates
	GLfloat* tangents = NULL; // array of tangent vectors
    GLuint* indices = NULL;
    printf("%i vertices  %i  faces\n", get_numvertices(), get_numfaces());
	if (mesh->HasPositions ()) {
		points = (GLfloat*)malloc (get_numvertices() * 3 * sizeof (GLfloat));
		for (int i = 0; i < get_numvertices(); i++) {
			const aiVector3D* vp = &(mesh->mVertices[i]);
			points[i * 3] = (GLfloat)vp->x;
			points[i * 3 + 1] = (GLfloat)vp->y;
			points[i * 3 + 2] = (GLfloat)vp->z;
            //printf("vertex %i  %f  %f  %f\n", i, points[3*i+0], points[3*i+1], points[3*i+2]);
		}
	}
	if (mesh->HasNormals ()) {
		normals = (GLfloat*)malloc (get_numvertices() * 3 * sizeof (GLfloat));
		for (int i = 0; i < get_numvertices(); i++) {
			const aiVector3D* vn = &(mesh->mNormals[i]);
			normals[i * 3] = (GLfloat)vn->x;
			normals[i * 3 + 1] = (GLfloat)vn->y;
			normals[i * 3 + 2] = (GLfloat)vn->z;
            //printf("normal %i  %f  %f  %f\n", i, normals[3*i+0], normals[3*i+1], normals[3*i+2]);
		}
	}
	if (mesh->HasTextureCoords (0)) {
		texcoords = (GLfloat*)malloc (get_numvertices() * 2 * sizeof (GLfloat));
		for (int i = 0; i < get_numvertices(); i++) {
			const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
			texcoords[i * 2] = (GLfloat)vt->x;
			texcoords[i * 2 + 1] = (GLfloat)vt->y;
            //printf("texcoords %i  %f  %f\n", i, texcoords[2*i+0], texcoords[2*i+1]);
		}
	}
/*	if (mesh->HasFaces()) {
        this->set_facefactor(mesh->mFaces[0].mNumIndices);
		indices = (GLuint*)malloc(get_numfaces() * get_facefactor() * sizeof (GLuint));
		for (int i = 0; i < get_numfaces(); ++i) {
		    for (int j = 0; j < get_facefactor(); ++j) {
               indices[get_facefactor()*i + j] = mesh->mFaces[i].mIndices[j]; 
            }
		}

	}*/
	if ( mesh->HasTangentsAndBitangents() ) {
		tangents = (GLfloat *)malloc( get_numvertices() * 4 * sizeof( GLfloat ) );
		for (int i = 0; i < get_numvertices(); i++) {
				const aiVector3D *tangent = &( mesh->mTangents[i] );
				const aiVector3D *bitangent = &( mesh->mBitangents[i] );
				const aiVector3D *normal = &( mesh->mNormals[i] );

				// put the three vectors into my vec3 struct format for doing maths
				vec3 t( tangent->x, tangent->y, tangent->z );
				vec3 n( normal->x, normal->y, normal->z );
				vec3 b( bitangent->x, bitangent->y, bitangent->z );
				// orthogonalise and normalise the tangent so we can use it in something
				// approximating a T,N,B inverse matrix
				vec3 t_i = normalise( t - n * dot( n, t ) );

				// get determinant of T,B,N 3x3 matrix by dot*cross method
				float det = ( dot( cross( n, t ), b ) );
				if ( det < 0.0f ) {
					det = -1.0f;
				} else {
					det = 1.0f;
				}
				// push back 4d vector for inverse tangent with determinant
				tangents[i * 4 + 0] = t_i.v[0];
				tangents[i * 4 + 1] = t_i.v[1];
				tangents[i * 4 + 2] = t_i.v[2];
				tangents[i * 4 + 3] = det;
		}

	}
    this->makevao(points, normals, texcoords, tangents, indices);


	
    free(points);
    free(normals);
    free(texcoords);
    free(tangents);
    free(indices);
	aiReleaseImport (scene);
	printf ("mesh loaded\n");
	
	return true;
}

bool malla::load_texture_rgb(const char *filename, const char *sampler_name){
	glActiveTexture(GL_TEXTURE0);
	int code = load_texture(filename, &tex_rgb);

	glUseProgram (this->shaderprog);
    printf("getuniformlocation(%u, %s)\n", this->shaderprog, sampler_name);
	texloc_rgb = glGetUniformLocation( this->shaderprog, sampler_name );
    printf("texloc_rgb = %i\n", texloc_rgb);
	assert( texloc_rgb > -1 );
	glUniform1i( texloc_rgb, 0);
    glUseProgram(0);
}

bool malla::load_texture_normal(const char *filename, const char *sampler_name){
	glActiveTexture(GL_TEXTURE1);
	int code = load_texture(filename, &tex_normal);

	glUseProgram (this->shaderprog);
    printf("getuniformlocation(%u, %s)\n", this->shaderprog, sampler_name);
	texloc_normal = glGetUniformLocation( this->shaderprog, sampler_name );
    printf("texloc_normal = %i\n", texloc_normal);
	assert( texloc_normal > -1 );
	glUniform1i( texloc_normal, 1 );
	glUseProgram(0);
}

bool malla::load_texture (const char* file_name, GLuint *tex) {
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

// gets
GLuint malla::get_vao(){
    return this->vao;
}

int malla::get_numvertices(){
    return this->nvertices;
}

int malla::get_numfaces(){
    return this->nfaces;
}

vec3 malla::get_pos(){
    return this->pos;
}

char* malla::get_filename(){
    return this->filename;
}

GLuint malla::get_facetype(){
    return this->FTYPE;
}

GLuint malla::get_facefactor(){
    return this->FFACTOR;
}

mat4 malla::get_matrix(){
    return this->M;
}

GLuint malla::get_matloc(){
    return this->matloc;
}





// sets
void malla::set_pos(vec3 p){
    this->pos = p;
}


void malla::set_numvertices(int nvertices){
    this->nvertices = nvertices;
}

void malla::set_numfaces(int nfaces){
    this->nfaces = nfaces;
}

void malla::set_facetype(GLuint FTYPE){
    this->FTYPE = FTYPE;
}

void malla::set_facefactor(GLuint FFACTOR){
    this->FFACTOR = FFACTOR;
}

void malla::set_matrix(mat4 M){
    this->M = M;
}

void malla::set_matloc(GLuint matloc){
    this->matloc = matloc;
}

void malla::set_shaderprog(GLuint sprog){
    this->shaderprog = sprog;
}