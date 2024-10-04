#ifndef APP_H
#define APP_H

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

    unsigned int vbo;
    unsigned int instanceVbo;
    unsigned int ebo;
    unsigned int vao;
    unsigned int dibo;

    unsigned int objShader;

    float camSens = 3.0f;
    float camSpeed = 15.0f;
    Camera camera;

    ModelInfo terrainInfo;
    int terrainVertsCountX = 100;
    int terrainVertsCountZ = 100;
    float terrainUnitLength = 1.0f;
    glm::mat4 terrainTransform;

    void loadModels();
    void loadObjects();
    void configureVAO();
    void configureCamera();
    void configureInputs();

public :
    App();
    void run();
};

#endif