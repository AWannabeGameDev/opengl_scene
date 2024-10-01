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

void App::setVAOFormat()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexBuffer(0, vbo, 0, 6 * sizeof(float));

    glVertexAttribFormat(VERTEX_POS_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(VERTEX_POS_ATTRIB_INDEX, 0);
    glEnableVertexAttribArray(VERTEX_POS_ATTRIB_INDEX);

    glVertexAttribFormat(VERTEX_COLOR_ATTRIB_INDEX, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
    glVertexAttribBinding(VERTEX_COLOR_ATTRIB_INDEX, 0);
    glEnableVertexAttribArray(VERTEX_COLOR_ATTRIB_INDEX);
}

App::App() :
    window{initialize(screenWidth, screenHeight, "OpenGL Scene", 4, 6)},
    objShader{createShaderProgram("../src/shaders/obj_vs.glsl", "../src/shaders/obj_fs.glsl")}
{
    glDebugMessageCallback(glDebugCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT);

    glViewport(0, 0, screenWidth, screenHeight);

    loadModels();
    setVAOFormat();
}

void App::run()
{
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glBindVertexArray(vao);
        glUseProgram(objShader);

        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)0);

        glfwSwapBuffers(window);
    }
}