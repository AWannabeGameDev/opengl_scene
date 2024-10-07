#ifndef APP_H
#define APP_H

#include <gl_util/util.h>
#include <gl_util/camera.h>
#include <gl_util/uniform_setter.h>
#include <gl_util/keyboard_input.h>
#include <gl_util/mouse_input.h>
#include <gl_util/lighting.h>

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
    float shininess;
};

class App
{
private :
    static constexpr unsigned int VERTEX_POS_ATTRIB_INDEX = 0,
    VERTEX_NORMAL_ATTRIB_INDEX = 1,
    VERTEX_COLOR_ATTRIB_INDEX = 2,
    VERTEX_TRANSFORM_ATTRIB_INDEX = 3,
    VERTEX_NORMAL_MATRIX_ATTRIB_INDEX = 7;

    static constexpr unsigned int VERTEX_DATA_BINDING_POINT = 0,
    INSTANCE_DATA_BINDING_POINT = 1;

    static constexpr unsigned int LIGHTS_UNI_BINDING = 0;

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
    unsigned int lightsUBO;

    unsigned int objShader;

    float camSens = 3.0f;
    float camSpeed = 50.0f;
    Camera camera;

    ModelInfo terrainInfo;
    int terrainVertsCountX = 100;
    int terrainVertsCountZ = 100;
    float terrainUnitLength = 5.0f;
    float terrainHeightScale = 70.0f;
    float terrainGenNoiseScale = 0.006f;
    glm::mat4 terrainTransform;
    glm::mat4 terrainNormalMatrix;

    DirectionalLight dirLight;
    float ambience = 0.2f;

    void loadModels();
    void loadTextures();
    void createObjects();
    void createLightSources();
    void configureVAO();
    void configureCamera();
    void configureInputs();

public :
    App();
    void run();
};

#endif