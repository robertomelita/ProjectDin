#ifndef MALLA_H
#define MALLA_H

using namespace std;

class malla{
    private:
        GLuint vao, vbo;
        int numvertices;
        glm::vec3 pos;
	glm::mat4 model;
	int matloc;
        char* filename;

    public:
        malla(char *filename);

        // gets
        GLuint getvao();
        int getnumvertices();
        glm::vec3 getpos();
        char* getfilename();
	int getmatloc();

        // sets
        void setvao(GLuint vao);
        void setnumvertices(int num);
        void setpos(glm::vec3 p);
        void setfilename(char *f);
	void setmatloc(GLuint shaderprog, const char* name);
	void model2shader(GLuint shaderprog);
	void printmodel();
};










#endif
