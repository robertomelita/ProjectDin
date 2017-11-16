#ifndef SUELO_H
#define SUELO_H

using namespace std;

class suelo
{
private:
	GLuint VAO, VBO, tex_rgb,tex_normal;
    int nvertices;
    glm::vec3 pos;
    glm::mat4 model;
    int matloc;
    char *filename;
    btRigidBody* body;
    GLfloat ConstA,ConstD,ConstS;
    GLint texloc_rgb, texloc_normal;

public:
	suelo(char *filename);
	 //gets
    int getNvertices();
    GLuint getVao();
    GLuint getVbo();
    glm::vec3 getPos();
    GLuint getTex();
    btRigidBody* getRigidBody();

    // sets
    void setLightConstants(GLfloat ConstA,GLfloat ConstD,GLfloat ConstS);
    void setVao(GLuint vao);
    void setNvertices(int num);
    void setPos(glm::vec3 p);
    void setFilename(char *f);
    void setMatloc(GLuint shaderprog, const char* name);
    void model2shader(GLuint shaderprog);
    void render(GLuint shader_programme);
    void transform(glm::vec3 posObj);
    bool load_texture (const char* file_name,GLuint *tex);
    void initPhysics(worldPhysics *world);
    bool load_texture_rgb(const char *filename, const char *sampler_name, GLuint* shaderprog);
    bool load_texture_normal(const char *filename, const char *sampler_name, GLuint* shaderprog);
};
#endif