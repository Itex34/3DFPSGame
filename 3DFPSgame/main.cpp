#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>


#include "Camera.hpp"
#include "PlayerController.hpp"
#include "Model.hpp"
#include "Physics.hpp"
#include "Shader.hpp"


struct GameVars {
    int screenWidth = 800, screenHeight = 600;

    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
    double fpsTime = 0.0f;
    int frameCount = 0;

    float lastX = 400, lastY = 300;
    float farPlane = 1000.0f;
    float nearPlane = 0.1f;
	float FOV = 80.0f;

	glm::vec3 startPos = glm::vec3(0.0f, 20.0f, 0.0f);

    bool firstMouse = true;
    bool cursorEnabled = false;
};

GameVars gameVars;
Physics physics;
PlayerController playerController(gameVars.startPos, physics);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    gameVars.screenWidth = width;
    gameVars.screenHeight = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (gameVars.cursorEnabled) return;
    playerController.processMouse(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        gameVars.cursorEnabled = false;
    }
}

void processInput(GLFWwindow* window) {
    double currentFrame = glfwGetTime();
    gameVars.deltaTime = currentFrame - gameVars.lastFrame;
    gameVars.lastFrame = currentFrame;

    playerController.update(window, gameVars.deltaTime);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        gameVars.cursorEnabled = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void updateFPSCounter(GLFWwindow* window) {
    gameVars.frameCount++;
    double currentTime = glfwGetTime();
    double elapsedTime = currentTime - gameVars.fpsTime;

    if (elapsedTime >= 1.0f) {
        double fps = gameVars.frameCount / elapsedTime;
        std::stringstream ss;
        ss << "Game window - FPS: " << fps;
        glfwSetWindowTitle(window, ss.str().c_str());

        gameVars.frameCount = 0;
        gameVars.fpsTime = currentTime;
    }
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(gameVars.screenWidth, gameVars.screenHeight, "Game window", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    Model model("assets/floor2.fbx");

    Shader shader("shaders/vertex.vert", "shaders/fragment.frag");

    gameVars.fpsTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(playerController.currentFov),
            (float)gameVars.screenWidth / (float)gameVars.screenHeight,
            gameVars.nearPlane, gameVars.farPlane);

        glm::mat4 view = playerController.getViewMatrix();
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        shader.use();
        shader.setMat4("model", modelMatrix);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        model.draw();

        updateFPSCounter(window);
        glfwSwapBuffers(window);
        glfwPollEvents();

        physics.update(gameVars.deltaTime);
    }

    glfwTerminate();
    return 0;
}
