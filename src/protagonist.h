#ifndef PROTAGONIST_H
#define PROTAGONIST_H
#include "worldPhysics.h"
class protagonist
{
private:
	int vida;
    GLuint VAO, VBO, tex_rgb,tex_normal;
    int poder;
    int nvertices;
    glm::vec3 pos;
    glm::mat4 model;
    int matloc;
    char *filename;
    btRigidBody* body;
    GLint texloc_rgb, texloc_normal;
    
public:
	protagonist(char *filename);

    //gets
    int getVida();
    int getPoder();
    int getNvertices();
    GLuint getVao();
    GLuint getVbo();
    GLuint getTex();
    btRigidBody* getRigidBody();
    glm::vec3 getPos();

    // sets
    void setVao(GLuint vao);
    void setNvertices(int num);
    void setPos(glm::vec3 p);
    void setFilename(char *f);
    void setMatloc(GLuint shaderprog, const char* name);
    void model2shader(GLuint shaderprog);
	void render(GLuint shader_programme);
    void transform(glm::vec3 posObj,float rot);
    bool load_texture (const char* file_name,GLuint *tex);
    void initPhysics(worldPhysics *world);
    void setRot(float angle , glm::vec3 vector);
    bool load_texture_rgb(const char *filename, const char *sampler_name, GLuint* shaderprog);
    bool load_texture_normal(const char *filename, const char *sampler_name, GLuint* shaderprog);
};

#endif
