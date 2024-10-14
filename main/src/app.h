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
    unsigned int diffuseID;
    unsigned int specularID;
    unsigned int normalID;
    float shininess;
};

class App
{
private :
    static constexpr unsigned int VERTEX_POS_ATTRIB_INDEX = 0,
    VERTEX_NORMAL_ATTRIB_INDEX = 1,
    VERTEX_TANGENT_ATTRIB_INDEX = 2,
    VERTEX_TEXCOORD_ATTRIB_INDEX = 3,
    VERTEX_TRANSFORM_ATTRIB_INDEX = 4,
    VERTEX_NORMAL_MATRIX_ATTRIB_INDEX = 8;

    static constexpr unsigned int VERTEX_DATA_BINDING_POINT = 0,
    INSTANCE_DATA_BINDING_POINT = 1;

    static constexpr unsigned int LIGHTS_UNI_BINDING = 0,
    CAMERA_MATRICES_UNI_BINDING = 1,
    DIRLIGHT_MATRIX_UNI_BINDING = 2;

    static constexpr int POSTCOLOR_TEXTURE_UNIT = 0,
    DIR_SHADOWMAP_TEXTURE_UNIT = 1,
    DIFFUSE_TEXTURE_UNIT = 2,
    SPECULAR_TEXTURE_UNIT = 3,
    NORMAL_TEXTURE_UNIT = 4;

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
    unsigned int cameraUBO;
    unsigned int dirLightMatrixUBO;

    unsigned int objShader;
    unsigned int normalShader;
    unsigned int dirShadowShader;

    unsigned int defaultNormalMap;

    float camSens = 1.5f;
    float camSpeed = 10.0f;
    Camera camera;

    ModelInfo terrainInfo;
    int terrainVertsCountX = 25;
    int terrainVertsCountZ = 25;
    int terrainVertsCount = terrainVertsCountX * terrainVertsCountZ;
    float terrainUnitLength = 5.0f;
    float terrainHeightScale = 13.0f;
    float terrainGenNoiseScale = 0.03f;
    glm::mat4 terrainTransform;
    glm::mat3 terrainNormalMatrix;

    ModelInfo cubeInfo;
    glm::mat4 cubeTransform;
    glm::mat3 cubeNormalMatrix;
    int cubeVertsCount = 24;

    DirectionalLight dirLight;
    glm::mat4 dirLightMatrix;
    unsigned int dirLightShadowMap;
    int shadowMapWidth = 4096;
    int shadowMapHeight = 4096;
    float ambience = 0.1f;

    unsigned int shadowFBO;

    unsigned int postFBO;
    unsigned int postFBOColorTexture;
    unsigned int postFBODepthRBO;
    unsigned int postShader;
    float gamma = 2.2f;

    void loadModels();
    void loadTextures();
    void createObjects();
    void createLightSources();
    void configureVAO();
    void configureCamera();
    void configurePostFBO();
    void configureInputs();

public :
    App();
    void run();
};

#endif