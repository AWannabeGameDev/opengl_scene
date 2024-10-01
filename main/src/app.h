#include <gl_util/util.h>

class App
{
    private :
        static constexpr unsigned int VERTEX_POS_ATTRIB_INDEX = 0,
        VERTEX_COLOR_ATTRIB_INDEX = 1;

        int screenWidth = 1280;
        int screenHeight = 720;
        GLFWwindow* window;

        unsigned int vbo;
        unsigned int ebo;
        unsigned int vao;

        unsigned int objShader;

        void loadModels();
        void setVAOFormat();

    public :
        App();
        void run();
};