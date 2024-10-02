#include <gl_util/util.h>
#include <gl_util/camera.h>
#include <gl_util/uniform_setter.h>
#include <gl_util/keyboard_input.h>
#include <gl_util/mouse_input.h>

struct DrawIndirect
{
    unsigned int indexCount;
    unsigned int instanceCount;
    unsigned int eboOffset;
    unsigned int vboOffset;
    unsigned int instanceOffset;
};

struct ModelInfo
{
    DrawIndirect drawCmd;
    unsigned int diffuseID;
    unsigned int specularID;
    unsigned int emissiveID;
    unsigned int normalID;
    unsigned int parallaxID;
};

class App
{
    private :
        static constexpr unsigned int VERTEX_POS_ATTRIB_INDEX = 0,
        VERTEX_COLOR_ATTRIB_INDEX = 1,
        VERTEX_TRANSFORM_ATTRIB_INDEX = 2;

        static constexpr unsigned int VERTEX_DATA_BINDING_POINT = 0,
        INSTANCE_DATA_BINDING_POINT = 1;

        int screenWidth = 1280;
        int screenHeight = 720;
        GLFWwindow* window;

        KeyboardInput keys;
        MouseInput mouse;

        UniformSetter uniforms;

        float camSens = 3.0f;
        float camSpeed = 8.0f;
        Camera camera;        

        unsigned int vbo;
        unsigned int instanceVbo;
        unsigned int ebo;
        unsigned int vao;

        unsigned int objShader;

        void loadModels();
        void loadObjects();
        void configureVAO();
        void configureCamera();
        void configureInputs();

    public :
        App();
        void run();
};