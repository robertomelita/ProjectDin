#ifndef SUELO_H
#define SUELO_H

using namespace std;

class suelo
{
private:
	GLuint VAO, VBO, tex;
    int nvertices;
    glm::vec3 pos;
    glm::mat4 model;
    int matloc;
    char *filename;
    btRigidBody* body;
    GLfloat ConstA,ConstD,ConstS;

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
    bool load_texture (const char* file_name);
    void initPhysics(worldPhysics *world);
};
#endif