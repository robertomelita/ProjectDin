#ifndef ENEMY_H
#define ENEMY_H

class enemy{
private:
    int vida;
    GLuint VAO, VBO, tex;
    int poder;
    int nvertices;
    glm::vec3 pos;
    glm::mat4 model;
    int matloc;
    char *filename;

public:
    enemy(char *filename);

    //gets
    int getVida();
    int getPoder();
    int getNvertices();
    GLuint getVao();
    GLuint getVbo();
    GLuint getTex();
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
    bool load_texture (const char* file_name);
};
#endif
