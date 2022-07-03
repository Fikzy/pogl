#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>

#include "camera.hh"
#include "model.hh"
#include "obj_loader.hh"
#include "shader.hh"
#include "utils.hh"

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void updateCameraRotation(GLFWwindow *window);
void processInput(GLFWwindow *window);

float SCREEN_W = 1920.0f / 2;
float SCREEN_H = 1080.0f / 2;
float FOV = 65.0f;
float SENSITIVITY = 0.1f;

const float NEAR_CLIP = 0.1f;
const float FAR_CLIP = 1000.0f;

double lastXPos = SCREEN_W / 2, lastYPos = SCREEN_H / 2;
double yaw = 0, pitch = 0, xPos, yPos;

Camera *camera;
glm::highp_mat4 projection;
std::map<int, bool> heldKeys;

int main()
{
    srand(time(NULL));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window =
        glfwCreateWindow(SCREEN_W, SCREEN_H, "POGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Camera
    camera = new Camera(glm::vec3(-8.0f, 2.0f, 0.0f));
    projection = glm::perspective(
        glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    // Shaders
    Shader untexturedModelShader("untextured.vert", "untextured.frag");
    Shader texturedModelShader("textured.vert", "textured.frag");

    // Models
    Model cube("assets/cube.obj", &untexturedModelShader);
    Model tree("assets/tree2.obj", &texturedModelShader,
               "assets/tree_palette.png");

    glClearColor(0, 0.1f, 0.2f, 0.8f);
    while (!glfwWindowShouldClose(window))
    {
        Camera::updateDeltaTime();

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Projection
        auto modelMatrix = glm::mat4(1.0);
        auto translatedModelMatrix =
            glm::translate(modelMatrix, glm::vec3(0, 0, 3));

        auto viewMatrix = camera->getWorldToViewMatrix();

        auto cubeMVP = projection * viewMatrix * modelMatrix;
        auto treeMVP = projection * viewMatrix * translatedModelMatrix;

        // Render
        cube.render(cubeMVP);
        tree.render(treeMVP);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void updateCameraRotation(GLFWwindow *window)
{
    glfwGetCursorPos(window, &xPos, &yPos);

    yaw += (xPos - lastXPos) * SENSITIVITY;
    pitch += (lastYPos - yPos) * SENSITIVITY;
    pitch = clamp(pitch, 89.0f, -89.0f);

    lastXPos = xPos;
    lastYPos = yPos;

    camera->rotate((float)yaw, (float)pitch);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods)
{
    if (action == GLFW_REPEAT)
        return;
    heldKeys[key] = action == GLFW_PRESS;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera controls
    updateCameraRotation(window);

    if (heldKeys[GLFW_KEY_SPACE])
        camera->moveUp(1.0f);
    if (heldKeys[GLFW_KEY_LEFT_CONTROL])
        camera->moveUp(-1.0f);
    if (heldKeys[GLFW_KEY_W])
        camera->moveForward(1.0f);
    if (heldKeys[GLFW_KEY_S])
        camera->moveForward(-1.0f);
    if (heldKeys[GLFW_KEY_D])
        camera->moveLeft(1.0f);
    if (heldKeys[GLFW_KEY_A])
        camera->moveLeft(-1.0f);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    SCREEN_W = (float)width;
    SCREEN_H = (float)height;
    projection = glm::perspective(
        glm::radians(FOV), (GLfloat)(SCREEN_W / SCREEN_H), NEAR_CLIP, FAR_CLIP);

    glViewport(0, 0, width, height);
}
