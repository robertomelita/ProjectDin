#ifndef PROTAGONIST_H
#define PROTAGONIST_H
class protagonist
{
private:
	int vida;
    GLuint VAO, VBO;
    int poder;
    int nvertices;
    glm::vec3 pos;
    glm::mat4 model;
    int matloc;
    char *filename;
public:
	protagonist(char *filename);

    //gets
    int getVida();
    int getPoder();
    int getNvertices();
    GLuint getVao();
    GLuint getVbo();
    glm::vec3 getPos();

    // sets
    void setVao(GLuint vao);
    void setNvertices(int num);
    void setPos(glm::vec3 p);
    void setFilename(char *f);
    void setMatloc(GLuint shaderprog, const char* name);
    void model2shader(GLuint shaderprog);
	
};

#endif
