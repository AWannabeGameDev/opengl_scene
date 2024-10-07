#include "app.h"
#define DB_PERLIN_IMPL
#include <gl_util/db_perlin.h>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct InstanceData
{
    glm::mat4 modelMatrix;
    glm::mat3 normalMatrix;
};

void App::loadModels()
{
    Vertex verts[terrainVertsCountX * terrainVertsCountZ];

    for(int z = 0; z < terrainVertsCountZ; z++)
    {
        for(int x = 0; x < terrainVertsCountX; x++)
        {
            int index = x + (z * terrainVertsCountX);
            float height = (db::perlin(x * terrainUnitLength * terrainGenNoiseScale, 
                                       z * terrainUnitLength * terrainGenNoiseScale) * 0.5f) + 1.0f;

            verts[index].position = {x * terrainUnitLength, height * terrainHeightScale, z * terrainUnitLength};
            verts[index].color = {height, height, height};

            // TODO: Generate normals
        }
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    unsigned int indices[6 * (terrainVertsCountX - 1) * (terrainVertsCountZ - 1)];

    for(int z = 0; z < (terrainVertsCountZ - 1); z++)
    {
        for(int x = 0; x < (terrainVertsCountX - 1); x++)
        {
            int elemIndex = 6 * (x + (z * (terrainVertsCountX - 1)));
            int vertIndex = x + (z * terrainVertsCountX);

            indices[elemIndex] = vertIndex;
            indices[elemIndex + 1] = vertIndex + 1;
            indices[elemIndex + 2] = vertIndex + terrainVertsCountX;
            indices[elemIndex + 3] = vertIndex + terrainVertsCountX;
            indices[elemIndex + 4] = vertIndex + 1;
            indices[elemIndex + 5] = vertIndex + terrainVertsCountX + 1;
        }
    }

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    terrainInfo.drawCmd.eboOffset = 0;
    terrainInfo.drawCmd.vboOffset = 0;
    terrainInfo.drawCmd.indexCount = 6 * (terrainVertsCountX - 1) * (terrainVertsCountZ - 1);
    terrainInfo.drawCmd.instanceCount = 1;
    terrainInfo.drawCmd.instanceOffset = 0;

    glGenBuffers(1, &dibo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, dibo);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawIndirect), &terrainInfo.drawCmd, GL_STATIC_DRAW);
}

void App::loadTextures()
{
    terrainInfo.shininess = 16.0f;

    uniforms.addUniform(objShader, "u_shininess");
}

void App::createObjects()
{
    terrainTransform = glm::mat4{1.0f};
    terrainNormalMatrix = glm::mat3{glm::inverse(glm::transpose(terrainTransform))};

    printf("[%f, %f, %f]\n", terrainNormalMatrix[0].x, terrainNormalMatrix[0].y, terrainNormalMatrix[0].z);
    printf("[%f, %f, %f]\n", terrainNormalMatrix[1].x, terrainNormalMatrix[1].y, terrainNormalMatrix[1].z);
    printf("[%f, %f, %f]\n", terrainNormalMatrix[2].x, terrainNormalMatrix[2].y, terrainNormalMatrix[2].z);

    glGenBuffers(1, &instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(terrainTransform));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::mat4), sizeof(glm::mat3), glm::value_ptr(terrainNormalMatrix));
}

void App::createLightSources()
{
    dirLight.direction = glm::normalize(glm::vec3{1.0f, -1.0f, 0.0f});
    dirLight.diffuseColor = {1.0f, 1.0f, 1.0f};
    dirLight.specularColor = {0.3f, 0.3f, 0.3f};

    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight), &dirLight, GL_DYNAMIC_DRAW);

    uniforms.bindUniformBlock(objShader, "lights", LIGHTS_UNI_BINDING);
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_UNI_BINDING, lightsUBO);

    uniforms.addUniform(objShader, "u_viewPosition");
    uniforms.addUniform(objShader, "u_ambience");

    glUseProgram(objShader);
    uniforms.setUniform(objShader, "u_ambience", ambience);
}

void App::configureVAO()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexBuffer(VERTEX_DATA_BINDING_POINT, vbo, 0, sizeof(Vertex));
    glBindVertexBuffer(INSTANCE_DATA_BINDING_POINT, instanceVbo, 0, sizeof(InstanceData));
    glVertexBindingDivisor(INSTANCE_DATA_BINDING_POINT, 1);

    glVertexAttribFormat(VERTEX_POS_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexAttribBinding(VERTEX_POS_ATTRIB_INDEX, VERTEX_DATA_BINDING_POINT);
    glEnableVertexAttribArray(VERTEX_POS_ATTRIB_INDEX);

    glVertexAttribFormat(VERTEX_NORMAL_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexAttribBinding(VERTEX_NORMAL_ATTRIB_INDEX, VERTEX_DATA_BINDING_POINT);
    glEnableVertexAttribArray(VERTEX_NORMAL_ATTRIB_INDEX);

    glVertexAttribFormat(VERTEX_COLOR_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
    glVertexAttribBinding(VERTEX_COLOR_ATTRIB_INDEX, VERTEX_DATA_BINDING_POINT);
    glEnableVertexAttribArray(VERTEX_COLOR_ATTRIB_INDEX);

    for(int i = 0; i < 4; i++)
    {
        glVertexAttribFormat(VERTEX_TRANSFORM_ATTRIB_INDEX + i, 4, GL_FLOAT, GL_FALSE, 
                             offsetof(InstanceData, modelMatrix) + (i * sizeof(glm::vec4)));
        glVertexAttribBinding(VERTEX_TRANSFORM_ATTRIB_INDEX + i, INSTANCE_DATA_BINDING_POINT);
        glEnableVertexAttribArray(VERTEX_TRANSFORM_ATTRIB_INDEX + i);
    }

    for(int i = 0; i < 3; i++)
    {
        glVertexAttribFormat(VERTEX_NORMAL_MATRIX_ATTRIB_INDEX + i, 3, GL_FLOAT, GL_FALSE, 
                             offsetof(InstanceData, normalMatrix) + (i * sizeof(glm::vec3)));
        glVertexAttribBinding(VERTEX_NORMAL_MATRIX_ATTRIB_INDEX + i, INSTANCE_DATA_BINDING_POINT);
        glEnableVertexAttribArray(VERTEX_NORMAL_MATRIX_ATTRIB_INDEX + i);
    }
}

void App::configureCamera()
{
    uniforms.addUniform(objShader, "u_view");
    uniforms.addUniform(objShader, "u_proj");

    glUseProgram(objShader);
    uniforms.setUniform(objShader, "u_proj", camera.projectionMatrix());
}

void App::configureInputs()
{
    keys.setKeybind("FORWARD", GLFW_KEY_W);
    keys.setKeybind("BACKWARD", GLFW_KEY_S);
    keys.setKeybind("LEFT", GLFW_KEY_A);
    keys.setKeybind("RIGHT", GLFW_KEY_D);
    keys.setKeybind("UP", GLFW_KEY_LEFT_SHIFT);
    keys.setKeybind("DOWN", GLFW_KEY_LEFT_CONTROL);
}

App::App() :
    window{initialize(screenWidth, screenHeight, "OpenGL Scene", 4, 6)},
    keys{window}, mouse{window},
    camera{glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f},
    objShader{createShaderProgram("../src/shaders/obj_vs.glsl", "../src/shaders/obj_fs.glsl")}
{
    glDebugMessageCallback(glDebugCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, screenWidth, screenHeight);

    loadModels();
    loadTextures();
    createObjects();
    createLightSources();
    configureVAO();
    configureCamera();
    configureInputs();
}

void App::run()
{
    glfwPollEvents();
    keys.update();
    mouse.update();

    float prevTime = 0.0f;

    while(!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        float dt = currentTime - prevTime;
        prevTime = currentTime;

        glfwPollEvents();
        keys.update();
        mouse.update();

        if(keys.keyPressed("FORWARD"))
        {
            camera.translate(-camera.behind() * camSpeed * dt);
        }
        else if(keys.keyPressed("BACKWARD"))
        {
            camera.translate(camera.behind() * camSpeed * dt);
        }

        if(keys.keyPressed("LEFT"))
        {
            camera.translate(-camera.right() * camSpeed * dt);
        }
        else if(keys.keyPressed("RIGHT"))
        {
            camera.translate(camera.right() * camSpeed * dt);
        }

        if(keys.keyPressed("UP"))
        {
            camera.translate(camera.up() * camSpeed * dt);
        }
        else if(keys.keyPressed("DOWN"))
        {
            camera.translate(-camera.up() * camSpeed * dt);
        }

        camera.rotateLocal({1.0f, 0.0f, 0.0f}, mouse.getMouseMovementY() * camSens * dt);
        camera.rotateGlobal({0.0f, 1.0f, 0.0f}, -mouse.getMouseMovementX() * camSens * dt);

        glUseProgram(objShader);
        uniforms.setUniform(objShader, "u_view", camera.viewMatrix());
        uniforms.setUniform(objShader, "u_viewPosition", camera.position());

        //----------------------------------------------------------------------------------------

        glBindVertexArray(vao);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, dibo);

        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        uniforms.setUniform(objShader, "u_shininess", terrainInfo.shininess);
        glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)0);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}