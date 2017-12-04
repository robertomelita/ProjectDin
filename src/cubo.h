#ifndef CUBO_H
#define CUBO_H
#include "worldPhysics.h"
class cubo
{
private:
	int vida;
    GLuint VAO, VBO, nbo, tbo, tanbo, ebo, tex_rgb, tex_normal;
    int poder;
    int nvertices;
    glm::vec3 pos;
    glm::mat4 model;
    int matloc;
    char *filename;
    btRigidBody* body;
    btRigidBody* cap;
    GLint texloc_rgb, texloc_normal;
    
public:
	cubo(char *filename);

    //gets
    int getNvertices();
    bool init = true;
    bool active = true;
    bool lockAxis = false;
    GLuint getVao();
    GLuint getVbo();
    GLuint getTex();
    btRigidBody* getRigidBody();
    btRigidBody* capsule();
    glm::vec3 getPos();

    // sets
    void setVao(GLuint vao);
    void setNvertices(int num);
    void setPos(glm::vec3 p);
    void setFilename(char *f);
    void setMatloc(GLuint shaderprog, const char* name);
    void model2shader(GLuint shaderprog);
	void render(GLuint shader_programme);
    void transform(glm::vec3 posObj);
    void initPhysics(worldPhysics *world);
    void manager();

    bool load_mesh(const char* file_name, int* point_count);
    bool load_texture (const char* file_name,GLuint *tex);
    bool load_texture_rgb(const char *filename, const char *sampler_name, GLuint* shaderprog);
    bool load_texture_normal(const char *filename, const char *sampler_name, GLuint* shaderprog);
    void makevao(GLfloat *vertices, GLfloat* normals, GLfloat* texcoords, GLfloat *tangents);
};

#endif
