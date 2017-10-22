#ifndef SUELO_H
#define SUELO_H

using namespace std;

class suelo
{
private:
	GLuint VAO, VBO;
    int nvertices;
    glm::vec3 pos;
    glm::mat4 model;
    int matloc;
    char *filename;
protected:
    GLuint tex; 
public:
	suelo(char *filename);
	 //gets
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
    
    bool load_texture (const char* file_name);
};
#endif