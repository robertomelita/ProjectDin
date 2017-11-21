#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl_utils.h"
#include "input.h"
#include "tools.h"

#define PI 3.14159265359
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

using namespace std;

bool load_mesh (const char* file_name, GLuint* vao, int* point_count) {
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
	glGenVertexArrays (1, vao);
	glBindVertexArray (*vao);
	
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
	
	/* copy mesh data into VBOs */
	if (mesh->HasPositions ()) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof (GLfloat),
			points,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (0);
		free (points);
	}
	if (mesh->HasNormals ()) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof (GLfloat),
			normals,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (1);
		free (normals);
	}
	if (mesh->HasTextureCoords (0)) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			2 * *point_count * sizeof (GLfloat),
			texcoords,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (2);
		free (texcoords);
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
				glm::vec3 t_i = glm::normalize( t - n * dot( n, t ) );

				// get determinant of T,B,N 3x3 matrix by dot*cross method
				float det = ( dot( cross( n, t ), b ) );
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
                free(tangents);
		}
	}
	
	aiReleaseImport (scene);
	printf ("mesh loaded\n");
	
	return true;
}

void init(int g_gl_width, int g_gl_height, GLuint *shader_programme){
	restart_gl_log ();
	start_gl ();
	glfwSwapInterval(1);
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable (GL_CULL_FACE); // cull face
	glCullFace (GL_BACK); // cull back face
	glFrontFace (GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor (0.2, 0.2, 0.2, 1.0); // grey background to help spot mistakes
	glViewport (0, 0, g_gl_width, g_gl_height);
	
	*shader_programme = create_programme_from_files (
		VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE
	);


	init_input();
	
		/*-------------------------------CREATE SHADERS-------------------------------*/
	world = new worldPhysics();

	sword = new protagonist((char*)"mallas/maraca2.obj");
	sword->load_texture_rgb("textures/ropa2.png", "texsamp_rgb",shader_programme);
	sword->load_texture_normal("textures/tula_packed0_normal.png", "texsamp_normal",shader_programme);
	sword->setPos(posObj);
	sword->initPhysics(world);
	sword->setMatloc(*shader_programme,"model");
	
	piso = new suelo((char*)"mallas/mapadm2.obj");
	piso->load_texture_rgb("textures/agua.jpg", "texsamp_rgb",shader_programme);
//	piso->load_texture_normal("textures/normal.jpg", "texsamp_normal",shader_programme);
	piso->setLightConstants(0.2f,0.8f,0.1f);
	piso->setPos(glm::vec3(0,-50.0f,0));
	piso->initPhysics(world);
	piso->setMatloc(*shader_programme,"model");

	castillo = new suelo((char*)"mallas/castillo.obj");
	castillo->load_texture_rgb("textures/earth8k.jpg","texsamp_rgb",shader_programme);
	castillo->load_texture_normal("textures/earth8k-normal.png", "texsamp_normal",shader_programme);
	castillo->setLightConstants(0.5f,0.8f,0.6f);
	castillo->setPos(glm::vec3(-25.0f,-35.0f,-150.0f));
	castillo->initPhysics(world);
	castillo->setMatloc(*shader_programme,"model");

	espada = new suelo((char*)"mallas/mastersword.obj");
	espada->load_texture_rgb("textures/espada.png", "texsamp_rgb",shader_programme);
	espada->load_texture_normal("textures/normal.jpg", "texsamp_normal",shader_programme);
	espada->setLightConstants(0.5f,0.8f,0.6f);
	espada->setPos(glm::vec3(-30.0f,-41.0f,-115.0f));
	espada->initPhysics(world);
	espada->setMatloc(*shader_programme,"model");

	arbolito = new suelo((char*)"mallas/arbolito.obj");
	arbolito->load_texture_rgb("textures/arbi.png", "texsamp_rgb",shader_programme);
	arbolito->load_texture_normal("textures/normal.jpg", "texsamp_normal",shader_programme);
	arbolito->setLightConstants(0.5,0.8f,0.6f);
	arbolito->setPos(glm::vec3(-40.0f,-48.0f,-115.0f));
	arbolito->setMatloc(*shader_programme,"model");

	key = new enemy((char*)"mallas/Key_B_02.obj");
	key->load_texture("textures/keyB_tx.bmp");
	key->setPos(glm::vec3(15.0f,-45.0f,-100.0f));
	key->setMatloc(*shader_programme,"model");


	projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 1000.0f);
	view = glm::lookAt(cameraPos, posObj, cameraUp);
	
	view_mat_location = glGetUniformLocation (*shader_programme, "view");
	glUseProgram (*shader_programme);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	proj_mat_location = glGetUniformLocation (*shader_programme, "proj");
	glUseProgram (*shader_programme);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);
}
