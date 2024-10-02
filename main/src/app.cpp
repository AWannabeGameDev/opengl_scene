#include "app.h"

void App::loadModels()
{
    float verts[] =
    {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    unsigned int indices[] = 
    {
        0, 1, 2
    };

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void App::loadObjects()
{
    glGenBuffers(1, &instanceVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
}

void App::configureVAO()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexBuffer(VERTEX_DATA_BINDING_POINT, vbo, 0, 6 * sizeof(float));
    glBindVertexBuffer(INSTANCE_DATA_BINDING_POINT, instanceVbo, 0, sizeof(glm::mat4));
    glVertexBindingDivisor(INSTANCE_DATA_BINDING_POINT, 1);

    glVertexAttribFormat(VERTEX_POS_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(VERTEX_POS_ATTRIB_INDEX, VERTEX_DATA_BINDING_POINT);
    glEnableVertexAttribArray(VERTEX_POS_ATTRIB_INDEX);

    glVertexAttribFormat(VERTEX_COLOR_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    glVertexAttribBinding(VERTEX_COLOR_ATTRIB_INDEX, VERTEX_DATA_BINDING_POINT);
    glEnableVertexAttribArray(VERTEX_COLOR_ATTRIB_INDEX);

    for(int i = 0; i < 4; i++)
    {
        glVertexAttribFormat(VERTEX_TRANSFORM_ATTRIB_INDEX + i, 4, GL_FLOAT, GL_FALSE, i * sizeof(glm::vec4));
        glVertexAttribBinding(VERTEX_TRANSFORM_ATTRIB_INDEX + i, INSTANCE_DATA_BINDING_POINT);
        glEnableVertexAttribArray(VERTEX_TRANSFORM_ATTRIB_INDEX + i);
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
    camera{glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f},
    objShader{createShaderProgram("../src/shaders/obj_vs.glsl", "../src/shaders/obj_fs.glsl")}
{
    glDebugMessageCallback(glDebugCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glViewport(0, 0, screenWidth, screenHeight);

    loadModels();
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

        //----------------------------------------------------------------------------------------

        glBindVertexArray(vao);

        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)0);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}