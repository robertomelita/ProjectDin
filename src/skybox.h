#ifndef SKYBOX_H
#define SKYBOX_H

class skybox{
    private:
        GLuint vao,vbo,cube_texture,cube_sp;
        int cube_V_location;
        int cube_P_location;

    public:
    skybox(glm::mat4 projection,glm::mat4 view);
    bool loadCubeMap(GLenum side_target, const char* file_name);
    void createCubeMap(const char* front,const char* back,const char* top,const char* bottom,const char* left,const char* right);
    void render(glm::mat4 view);
};

#endif