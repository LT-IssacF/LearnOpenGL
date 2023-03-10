#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader_s.h>
#include <camera.h>
#include <iostream>
#include <cmath>

const GLuint _width = 800, _height = 600;
Camera camera(0.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f);
GLfloat deltaTime = 0.0f, lastFrame = 0.0f, lastX = _width / 2.0f, lastY = _height / 2.0;
bool firstMouse = true;
glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double offsetX, double offsetY);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(_width, _height, "Basic_lighting", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return 0;
    } glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress))) {
        std::cout << "Failed to load GLAD" << std::endl;
        return 0;
    } glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glEnable(GL_DEPTH_TEST);

    Shader objShader("../src/12_basic_lighting/shader/obj.vs.glsl", "../src/12_basic_lighting/shader/obj.fs.glsl"), // 物体渲染
    lightShader("../src/12_basic_lighting/shader/light.vs.glsl", "../src/12_basic_lighting/shader/light.fs.glsl"); // 光源渲染
    // 数据由VAO管理的VBO缓冲，传入vertexShader，再可以由vertexShader传入fragmentShader
    GLfloat vertices[] = { // 光源与物体共用一组顶点数据，可以通过后面的model矩阵加以区分
        //    position           normal vector
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    }; GLuint lightVAO, objVAO, VBO; // VBO是一块储存数据的缓冲，VAO是负责怎么解释这块数据的规则，故需要两个不同的VAO和一个共同的VBO
    glGenVertexArrays(1, &objVAO); // 创建、绑定、传输、指派、启用一条龙
    glGenBuffers(1, &VBO);
    glBindVertexArray(objVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // GL_ARRAY_BUFFER是这块缓冲的类型
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0); // layout (location = 0) aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // layout (location = 1) aNormal
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &lightVAO); // 光源用不上法向量，就可以直接无视法向量数据
    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glm::mat4 model, view, projection;
    while(!glfwWindowShouldClose(window)) {
        //pre calculate delta time
        GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window);
        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        objShader.use();
        objShader.setVec3("objColor", glm::vec3(1.0f, 0.5f, 0.31f));
        objShader.setVec3("lightColor", glm::vec3(1.0f));
        objShader.setVec3("lightPos", lightPos);
        objShader.setVec3("viewPos", camera.position);
        projection = glm::perspective(glm::radians(camera.fov), static_cast<GLfloat>(_width / _height), 0.1f, 100.0f);
        objShader.setMat4("projection", projection);
        view = camera.getViewMatrix();
        objShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        objShader.setMat4("model", model);

        glBindVertexArray(objVAO); // 先使用物体的解释规则
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);

        glBindVertexArray(lightVAO); // 再使用光源的解释规则
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // swap pixel
        glfwSwapBuffers(window);
        glfwPollEvents();
    } glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &objVAO);
    glDeleteBuffers(1, &VBO);
    lightShader.deleteProgram();
    objShader.deleteProgram();
    glfwTerminate();
    return 0;
} void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
} void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else {
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.processKeyboard(FORWARD, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.processKeyboard(BACKWARD, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.processKeyboard(LEFT, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.processKeyboard(RIGHT, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.processKeyboard(UP, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            camera.processKeyboard(DOWN, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            camera.processKeyboard(LOOK_LEFT, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            camera.processKeyboard(LOOK_RIGHT, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { // key P
            camera.processKeyboard(LOOK_UP, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { // key L
            camera.processKeyboard(LOOK_DOWN, deltaTime);
        }
    }
} void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos, lastY = ypos;
        firstMouse = false;
    } camera.processMouseMovement(xpos - lastX, lastY - ypos);
    lastX = xpos, lastY = ypos;
} void scroll_callback(GLFWwindow *window, double offsetX, double offsetY) {
    camera.processMouseScroll(offsetY);
}
