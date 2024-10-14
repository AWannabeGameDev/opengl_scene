#include "app.h"
#define DB_PERLIN_IMPL
#include <gl_util/db_perlin.h>
#include <glm/gtc/type_ptr.hpp>
#include <gl_util/texture.h>
#include <gl_util/raw_model_data.h>

using namespace models;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texCoord;
};

struct InstanceData
{
    glm::mat4 modelMatrix;
    glm::mat3 normalMatrix;
};

struct CameraMatrices
{
    glm::mat4 view;
    glm::mat4 proj;
};

void App::loadModels()
{
    terrainInfo.drawCmd.eboOffset = 0;
    terrainInfo.drawCmd.vboOffset = 0;
    terrainInfo.drawCmd.indexCount = 6 * (terrainVertsCountX - 1) * (terrainVertsCountZ - 1);
    terrainInfo.drawCmd.instanceCount = 1;
    terrainInfo.drawCmd.instanceOffset = 0;

    cubeInfo.drawCmd.eboOffset = terrainInfo.drawCmd.indexCount;
    cubeInfo.drawCmd.vboOffset = terrainVertsCount;
    cubeInfo.drawCmd.indexCount = cube::NUM_INDICES;
    cubeInfo.drawCmd.instanceCount = 1;
    cubeInfo.drawCmd.instanceOffset = 1;

    glGenBuffers(1, &dibo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, dibo);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, 2 * sizeof(DrawIndirect), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawIndirect), &terrainInfo.drawCmd);
    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawIndirect), sizeof(DrawIndirect), &cubeInfo.drawCmd);

    Vertex verts[terrainVertsCount + cube::NUM_VERTS];
    unsigned int indices[terrainInfo.drawCmd.indexCount + cubeInfo.drawCmd.indexCount];

    for(int z = 0; z < terrainVertsCountZ; z++)
    {
        for(int x = 0; x < terrainVertsCountX; x++)
        {
            int index = x + (z * terrainVertsCountX);
            float height = (db::perlin(x * terrainUnitLength * terrainGenNoiseScale, 
                                       z * terrainUnitLength * terrainGenNoiseScale) * 0.5f) + 1.0f;

            verts[index].position = {x * terrainUnitLength, height * terrainHeightScale, z * terrainUnitLength};
            verts[index].texCoord = glm::vec2{verts[index].position.x, verts[index].position.z} / 20.0f;
            verts[index].normal = {0.0f, 0.0f, 0.0f};
        }
    }

    for(int z = 0; z < (terrainVertsCountZ - 1); z++)
    {
        for(int x = 0; x < (terrainVertsCountX - 1); x++)
        {
            int tri1vert1 = x + (z * terrainVertsCountX);
            int tri1vert2 = tri1vert1 + 1;
            int tri1vert3 = tri1vert1 + terrainVertsCountX;
            int tri2vert1 = tri1vert3;
            int tri2vert2 = tri1vert2;
            int tri2vert3 = tri1vert1 + terrainVertsCountX + 1;

            glm::vec3 tri1normal = glm::normalize(glm::cross(verts[tri1vert2].position - verts[tri1vert1].position,
                                                             verts[tri1vert2].position - verts[tri1vert3].position));
            glm::vec3 tri2normal = glm::normalize(glm::cross(verts[tri2vert2].position - verts[tri2vert1].position,
                                                             verts[tri2vert2].position - verts[tri2vert3].position));

            verts[tri1vert1].normal += tri1normal;
            verts[tri1vert2].normal += tri1normal;
            verts[tri1vert3].normal += tri1normal;
            verts[tri2vert1].normal += tri2normal;
            verts[tri2vert2].normal += tri2normal;
            verts[tri2vert3].normal += tri2normal;

            int elemIndex = 6 * (x + (z * (terrainVertsCountX - 1)));
            indices[elemIndex] = tri1vert1;
            indices[elemIndex + 1] = tri1vert2;
            indices[elemIndex + 2] = tri1vert3;
            indices[elemIndex + 3] = tri2vert1;
            indices[elemIndex + 4] = tri2vert2;
            indices[elemIndex + 5] = tri2vert3;
        }
    }

    for(int index = 0; index < (terrainVertsCountX * terrainVertsCountZ); index++)
    {
        verts[index].normal = glm::normalize(verts[index].normal);
        verts[index].tangent = glm::normalize(glm::cross(verts[index].normal, {0.0f, 0.0f, 1.0f}));
    }

    glm::vec2 cubeTexCoords[4] = {{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}};

    glm::vec3 cubeTangents[3] =
    {
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f}
    };

    for(int index = terrainVertsCount; index < (terrainVertsCount + cube::NUM_VERTS); index++)
    {
        verts[index].position = cube::positions[index - terrainVertsCount];
        verts[index].normal = cube::normals[index - terrainVertsCount];
        verts[index].tangent = cubeTangents[(int)((index - terrainVertsCount) / 8.0f)];
        verts[index].texCoord = cubeTexCoords[(index - terrainVertsCount) % 4];
    }

    std::copy(cube::indices, cube::indices + cube::NUM_INDICES, indices + terrainInfo.drawCmd.indexCount);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void App::loadTextures()
{
    terrainInfo.shininess = 16.0f;
    cubeInfo.shininess = 32.0f;

    TextureParameterSet texParams =
    {
        .minFilter = GL_LINEAR_MIPMAP_LINEAR,
        .magFilter = GL_LINEAR,
        .texWrapS = GL_REPEAT,
        .texWrapT = GL_REPEAT,
        .texWrapR = GL_REPEAT
    };

    terrainInfo.diffuseID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/grass.png", true);
    terrainInfo.normalID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB, "../res/grass_normal.png", true);

    texParams.minFilter = GL_NEAREST;
    texParams.magFilter = GL_NEAREST;

    terrainInfo.specularID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB16, 1, 1);
    cubeInfo.diffuseID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB16, 1, 1);
    cubeInfo.specularID = createTexture(GL_TEXTURE_2D, texParams, GL_RGB16, 1, 1);
    defaultNormalMap = createTexture(GL_TEXTURE_2D, texParams, GL_RGB16, 1, 1);

    glm::vec3 terrainSpecularColor = {0.1f, 0.1f, 0.1f};
    glm::vec3 cubeDiffuseColor = {1.0f, 0.0f, 0.0f};
    glm::vec3 cubeSpecularColor = {0.5f, 0.5f, 0.5f};
    glm::vec3 defaultNormalColor = {0.5f, 0.5f, 1.0f};

    glBindTexture(GL_TEXTURE_2D, terrainInfo.specularID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, (const void*)&terrainSpecularColor);

    glBindTexture(GL_TEXTURE_2D, cubeInfo.diffuseID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, (const void*)&cubeDiffuseColor);

    glBindTexture(GL_TEXTURE_2D, cubeInfo.specularID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, (const void*)&cubeSpecularColor);

    glBindTexture(GL_TEXTURE_2D, defaultNormalMap);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, (const void*)&defaultNormalColor);

    cubeInfo.normalID = defaultNormalMap;

    uniforms.addUniform(objShader, "u_shininess");
    uniforms.addUniform(objShader, "u_diffuse");
    uniforms.addUniform(objShader, "u_specular");
    uniforms.addUniform(objShader, "u_normal");

    glUseProgram(objShader);
    uniforms.setUniform(objShader, "u_diffuse", DIFFUSE_TEXTURE_UNIT);
    uniforms.setUniform(objShader, "u_specular", SPECULAR_TEXTURE_UNIT);
    uniforms.setUniform(objShader, "u_normal", NORMAL_TEXTURE_UNIT);
}

void App::createObjects()
{
    terrainTransform = glm::mat4{1.0f};
    terrainNormalMatrix = glm::mat3{glm::inverse(glm::transpose(terrainTransform))};

    cubeTransform = glm::translate(glm::mat4{1.0f}, {25.0f, 20.0f, 25.0f})
                    * glm::scale(glm::mat4{1.0f}, {4.0f, 4.0f, 4.0f});
    cubeNormalMatrix = glm::mat3{glm::inverse(glm::transpose(cubeTransform))};

    glGenBuffers(1, &instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, offsetof(InstanceData, modelMatrix), sizeof(glm::mat4), glm::value_ptr(terrainTransform));
    glBufferSubData(GL_ARRAY_BUFFER, offsetof(InstanceData, normalMatrix), sizeof(glm::mat3), 
                    glm::value_ptr(terrainNormalMatrix));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(InstanceData) + offsetof(InstanceData, modelMatrix), sizeof(glm::mat4), 
                    glm::value_ptr(cubeTransform));
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(InstanceData) + offsetof(InstanceData, normalMatrix), sizeof(glm::mat3), 
                    glm::value_ptr(cubeNormalMatrix));
}

void App::createLightSources()
{
    dirLight.direction = glm::normalize(glm::vec3{1.0f, -1.0f, 2.0f});
    dirLight.diffuseColor = {0.8f, 0.8f, 0.8f};
    dirLight.specularColor = {0.5f, 0.5f, 0.5f};

    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLight), &dirLight, GL_DYNAMIC_DRAW);

    uniforms.bindUniformBlock(objShader, "lights", LIGHTS_UNI_BINDING);
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTS_UNI_BINDING, lightsUBO);

    glm::vec3 dirLightPosition = 
    {
        terrainVertsCountX * terrainUnitLength / 2.0f, 0.0f,
        terrainVertsCountZ * terrainUnitLength / 2.0f  
    };
    dirLightMatrix = glm::ortho(-terrainVertsCountX * terrainUnitLength / 1.3f, terrainVertsCountX * terrainUnitLength / 1.3f,
                                -terrainHeightScale * 1.5f, terrainHeightScale * 3.0f, 
                                -terrainVertsCountZ * terrainUnitLength, terrainVertsCountZ * terrainUnitLength)
                     * glm::lookAt(dirLightPosition, dirLightPosition + dirLight.direction, {0.0f, 1.0f, 0.0f});

    glGenBuffers(1, &dirLightMatrixUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, dirLightMatrixUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), glm::value_ptr(dirLightMatrix), GL_DYNAMIC_DRAW);

    uniforms.bindUniformBlock(dirShadowShader, "dirLightMatrix", DIRLIGHT_MATRIX_UNI_BINDING);
    uniforms.bindUniformBlock(objShader, "dirLightMatrix", DIRLIGHT_MATRIX_UNI_BINDING);
    glBindBufferBase(GL_UNIFORM_BUFFER, DIRLIGHT_MATRIX_UNI_BINDING, dirLightMatrixUBO);

    TextureParameterSet texParams =
    {
        .minFilter = GL_LINEAR,
        .magFilter = GL_LINEAR,
        .texWrapS = GL_CLAMP_TO_EDGE,
        .texWrapT = GL_CLAMP_TO_EDGE,
        .texWrapR = GL_CLAMP_TO_EDGE
    };
    dirLightShadowMap = createTexture(GL_TEXTURE_2D, texParams, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);

    glActiveTexture(GL_TEXTURE0 + DIR_SHADOWMAP_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_2D, dirLightShadowMap);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirLightShadowMap, 0);
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    uniforms.addUniform(objShader, "u_viewPosition");
    uniforms.addUniform(objShader, "u_ambience");
    uniforms.addUniform(objShader, "u_dirLightShadowMap");

    glUseProgram(objShader);
    uniforms.setUniform(objShader, "u_ambience", ambience);
    uniforms.setUniform(objShader, "u_dirLightShadowMap", DIR_SHADOWMAP_TEXTURE_UNIT);
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

    glVertexAttribFormat(VERTEX_TANGENT_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
    glVertexAttribBinding(VERTEX_TANGENT_ATTRIB_INDEX, VERTEX_DATA_BINDING_POINT);
    glEnableVertexAttribArray(VERTEX_TANGENT_ATTRIB_INDEX);

    glVertexAttribFormat(VERTEX_TEXCOORD_ATTRIB_INDEX, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
    glVertexAttribBinding(VERTEX_TEXCOORD_ATTRIB_INDEX, VERTEX_DATA_BINDING_POINT);
    glEnableVertexAttribArray(VERTEX_TEXCOORD_ATTRIB_INDEX);

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
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraMatrices), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, offsetof(CameraMatrices, proj), sizeof(glm::mat4), 
                    glm::value_ptr(camera.projectionMatrix()));

    uniforms.bindUniformBlock(objShader, "cameraMatrices", CAMERA_MATRICES_UNI_BINDING);
    uniforms.bindUniformBlock(normalShader, "cameraMatrices", CAMERA_MATRICES_UNI_BINDING);
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_MATRICES_UNI_BINDING, cameraUBO);
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
    objShader{createShaderProgram("../src/shaders/obj_vs.glsl", "../src/shaders/obj_fs.glsl")},
    normalShader{createShaderProgram("../src/shaders/normal_vs.glsl", "../src/shaders/normal_gs.glsl", 
                                     "../src/shaders/normal_fs.glsl")},
    dirShadowShader{createShaderProgram("../src/shaders/dir_shadowmap_vs.glsl", "../src/shaders/dir_shadowmap_fs.glsl")}
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
        uniforms.setUniform(objShader, "u_viewPosition", camera.position());

        glm::mat4 viewMatrix = camera.viewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, offsetof(CameraMatrices, view), sizeof(glm::mat4), 
                        glm::value_ptr(viewMatrix));

        /*
        dirLight.direction = glm::vec3{glm::rotate(glm::mat4{1.0f}, 0.5f * dt, {0.0f, 0.0f, 1.0f}) 
                                       * glm::vec4{dirLight.direction, 1.0f}};

        glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, offsetof(DirectionalLight, direction), sizeof(glm::vec3), 
                        glm::value_ptr(dirLight.direction));
        */

        //----------------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glViewport(0, 0, shadowMapWidth, shadowMapHeight);

        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(dirShadowShader);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)0, 2, sizeof(DrawIndirect));

        //----------------------------------------------------------------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);

        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(objShader);

        glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, terrainInfo.diffuseID);
        glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, terrainInfo.specularID);
        glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, terrainInfo.normalID);

        uniforms.setUniform(objShader, "u_shininess", terrainInfo.shininess);
        glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)0);

        glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, cubeInfo.diffuseID);
        glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, cubeInfo.specularID);
        glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
        glBindTexture(GL_TEXTURE_2D, cubeInfo.normalID);

        uniforms.setUniform(objShader, "u_shininess", cubeInfo.shininess);
        glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)sizeof(DrawIndirect));

        #if 0 
        glUseProgram(normalShader);
        //glDisable(GL_DEPTH_TEST);
        glDrawArraysInstancedBaseInstance(GL_POINTS, terrainInfo.drawCmd.vboOffset, terrainVertsCount, 1, 
                                          terrainInfo.drawCmd.instanceOffset);
        glDrawArraysInstancedBaseInstance(GL_POINTS, cubeInfo.drawCmd.vboOffset, cube::NUM_VERTS, 1,
                                          cubeInfo.drawCmd.instanceCount);
        #endif

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}