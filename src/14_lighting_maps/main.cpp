#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader_s.h>
#include <stb_image.h>
#include <camera.h>
#include <iostream>
#include <cmath>

const GLuint _width = 800, _height = 600;
Camera camera(0.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f);
GLfloat deltaTime = 0.0f, lastFrame = 0.0f, lastX = _width / 2.0f, lastY = _height / 2.0, brightness = 1.0f;
bool firstMouse = true;
glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f), lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
// glm::vec3 lightPos = glm::vec3(-0.45f, 1.4f, 2.5f);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double offsetX, double offsetY);
GLuint loadTexture(const char *path);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(_width, _height, "Materials", NULL, NULL);
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

    Shader objShader("../src/14_lighting_maps/shader/obj.vs.glsl", "../src/14_lighting_maps/shader/obj.fs.glsl"), // 物体渲染
    lightShader("../src/14_lighting_maps/shader/light.vs.glsl", "../src/14_lighting_maps/shader/light.fs.glsl"); // 光源渲染
    // 数据由VAO管理的VBO缓冲，传入vertexShader，再可以由vertexShader传入fragmentShader
    GLfloat vertices[] = { // 光源与物体共用一组顶点数据，可以通过后面的model矩阵加以区分
        //    position          normal vector     texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    }; GLuint lightVAO, objVAO, VBO; // VBO是一块储存数据的缓冲，VAO是负责怎么解释这块数据的规则，故需要两个不同的VAO和一个共同的VBO
    glGenVertexArrays(1, &objVAO); // 创建、绑定、传输、指派、启用一条龙
    glGenBuffers(1, &VBO);
    glBindVertexArray(objVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // GL_ARRAY_BUFFER是这块缓冲的类型
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0); // layout (location = 0) aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // layout (location = 1) aNormal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // layout (location = 2) aTexCoords
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &lightVAO); // 光源用不上法向量，就可以直接无视法向量数据
    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint diffuse = loadTexture("../resource/texture/container2.png"), // 环境光与漫反射共用的纹理
    specular = loadTexture("../resource/texture/container2_specular.png"); // 高光纹理
    // GLuint matrix = loadTexture("../resource/texture/matrix.jpg"); // exercise: emission map
    objShader.use();
    objShader.setInt("material.diffuse", 0); // 只有一张纹理时，默认绑定至0号纹理单元上，无需设置
    objShader.setInt("material.specular", 1); // 但此例有两张纹理
    // objShader.setInt("material.matrix", 2); // exercise: emission map

    glm::mat4 model, view, projection;
    glm::vec3 ambientColor, diffuseColor;
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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular);
        // glActiveTexture(GL_TEXTURE2); // exercise: emission map
        // glBindTexture(GL_TEXTURE_2D, matrix); // exercise: emission map
        objShader.setFloat("material.shininess", 32.0f);
        objShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f); 
        objShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        objShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        objShader.setVec3("light.position", lightPos);

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

        lightShader.setVec3("lightColor", lightColor);
        lightShader.setFloat("brightness", brightness);

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
} GLuint loadTexture(const char *path) {
    GLuint ID;
    glGenTextures(1, &ID);
    GLint width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if(data) {
        GLenum format;
        if(nrChannels == 1) {
            format = GL_RED;
        } else if(nrChannels == 3) {
            format = GL_RGB;
        } else if(nrChannels == 4) { // .png为四通道
            format = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    } stbi_image_free(data);
    return ID;
}
