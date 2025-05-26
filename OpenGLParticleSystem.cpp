#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Particle.h"
#include "Camera.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

GLuint compileShader(const char* vertexPath, const char* fragmentPath) {
    std::string vCode = loadShaderSource(vertexPath);
    std::string fCode = loadShaderSource(fragmentPath);

    const char* vShaderCode = vCode.c_str();
    const char* fShaderCode = fCode.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderProgram;
}

float floorVertices[] = {
    -5.0f, 0.0f, -5.0f,
     5.0f, 0.0f, -5.0f,
     5.0f, 0.0f,  5.0f,

    -5.0f, 0.0f, -5.0f,
     5.0f, 0.0f,  5.0f,
    -5.0f, 0.0f,  5.0f,
};

Camera camera(glm::vec3(0.0f, 5.0f, 15.0f));
float lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // odwrotnie, bo y idzie w górę

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
    float current = glfwGetTime();
    deltaTime = current - lastFrame;
    lastFrame = current;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particle System", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }
    glEnable(GL_DEPTH_TEST);


    GLuint floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint floorShader = compileShader("shaders/floor.vert", "shaders/floor.frag");
    GLuint particleShader = compileShader("shaders/particle.vert", "shaders/particle.frag");

    std::vector<Particle> particles;
    srand(static_cast<unsigned>(time(0)));

    glEnable(GL_PROGRAM_POINT_SIZE);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        float currentTime = static_cast<float>(glfwGetTime());
        static float lastTime = currentTime;
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Macierze

        glm::mat4 view = camera.GetViewMatrix();


        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 model = glm::mat4(1.0f); // Identity matrix for the floor

        GLuint modelLoc = glGetUniformLocation(floorShader, "model");
        GLuint viewLoc = glGetUniformLocation(floorShader, "view");
        GLuint projLoc = glGetUniformLocation(floorShader, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));



        glUseProgram(floorShader);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Update and spawn particles
        for (auto& p : particles)
            p.update(deltaTime);

        particles.erase(std::remove_if(particles.begin(), particles.end(), [](Particle& p) {
            return p.isDead();
            }), particles.end());

        // Add new particle
        particles.emplace_back(
            glm::vec3((rand() % 100 - 50) / 10.0f, 5.0f, (rand() % 100 - 50) / 10.0f),
            glm::vec3(0.0f, -2.0f, 0.0f),
            3.0f
        );

        GLuint pModelLoc = glGetUniformLocation(particleShader, "model");
        GLuint pViewLoc = glGetUniformLocation(particleShader, "view");
        GLuint pProjLoc = glGetUniformLocation(particleShader, "projection");

        glUniformMatrix4fv(pViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(pProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

        for (const auto& p : particles) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), p.position);
            glUniformMatrix4fv(pModelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_POINTS, 0, 1);
        }



        // Draw particles
        glUseProgram(particleShader);
        for (const auto& p : particles) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), p.position);
            GLuint modelLoc = glGetUniformLocation(particleShader, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_POINTS, 0, 1);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);

    glfwTerminate();
    return 0;
}
