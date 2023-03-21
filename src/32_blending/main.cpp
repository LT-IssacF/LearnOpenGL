#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <stb_image.h>
#include <camera.h>
#include <iostream>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <cmath>

const GLuint _width = 1680, _height = 1050;
Camera camera(0.0f, 0.0f, 3.0f, 0.0f, 1.0f, 0.0f);
GLfloat deltaTime = 0.0f, lastFrame = 0.0f, lastX = _width / 2.0f, lastY = _height / 2.0f;
bool firstMouse = true;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double offsetX, double offsetY);
GLuint loadTexture(const char *path);
bool cmp(std::pair<glm::vec3, float> &lhs, std::pair<glm::vec3, float> &rhs);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(_width, _height, "Blending", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to creat window" << std::endl;
        glfwTerminate();
        return 0;
    } glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress))) {
        std::cout << "Failed to load GLAD" << std::endl;
        return 0;
    } glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat cubeVertices[] = { // 按照逆时针环绕顺序定义
        // positions        texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left      
    }; GLfloat floorVertices[] = {
        // positions        texture Coords 
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    }; GLfloat objVertices[] = {
        // positions        texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    }; std::vector<glm::vec3> grassPos {
        glm::vec3( 1.5f, 0.0f, 0.51f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
    }; std::vector<glm::vec3> windowPos {
        glm::vec3( 0.0f, 0.0f, 0.7f),
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3 (0.5f, 0.0f, -0.6f)
    }; GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint objVAO, objVBO;
    glGenVertexArrays(1, &objVAO);
    glGenBuffers(1, &objVBO);
    glBindVertexArray(objVAO);
    glBindBuffer(GL_ARRAY_BUFFER, objVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(objVertices), objVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader shader("../src/32_blending/shader/grass.vs.glsl", "../src/32_blending/shader/grass.fs.glsl");
    shader.use();
    shader.setInt("texture1", 0);
    GLuint cubeTexture = loadTexture("../resource/texture/marble.jpg"),
    floorTexture = loadTexture("../resource/texture/metal.png"),
    grassTexture = loadTexture("../resource/texture/grass.png"),
    windowTexture = loadTexture("../resource/texture/window.png");

    while(!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        std::vector<std::pair<glm::vec3, float>> windowOrder, grassOrder; // 给要绘制的玻璃和草按离摄像机位置距离排序
        for(int i = 0; i < windowPos.size(); i++) {
            windowOrder.emplace_back(std::pair<glm::vec3, float>(windowPos[i], glm::length(camera.position - windowPos[i])));
        } std::sort(windowOrder.begin(), windowOrder.end(), cmp);
        for(int i = 0; i < grassPos.size(); i++) {
            grassOrder.emplace_back(std::pair<glm::vec3, float>(grassPos[i], glm::length(camera.position - grassPos[i])));
        } std::sort(grassOrder.begin(), grassOrder.end(), cmp);

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)_width / (float)_height, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // cube
        glEnable(GL_CULL_FACE); // 渲染前启用面剔除
        glCullFace(GL_BACK); // 剔除背向面
        glFrontFace(GL_CCW); // 逆时针环绕顺序定义为正向面
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDisable(GL_CULL_FACE); // 渲染完禁用面剔除
        // floor
        glBindVertexArray(floorVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // grass
        glBindVertexArray(objVAO);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        for(std::vector<std::pair<glm::vec3, float>>::reverse_iterator it = grassOrder.rbegin(); it != grassOrder.rend(); it++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->first);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // window
        glBindVertexArray(objVAO);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        // 但绘制半透明的窗户肯定要按距离由远至近绘制
        for(std::vector<std::pair<glm::vec3, float>>::reverse_iterator it = windowOrder.rbegin(); it != windowOrder.rend(); it++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->first);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    } glfwTerminate();
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
    stbi_set_flip_vertically_on_load(false);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if(data) {
        GLenum format;
        if(nrChannels == 1) {
            format = GL_RED;
        } else if(nrChannels == 3) {
            format = GL_RGB;
        } else if(nrChannels == 4) { // .png为四通道
            format = GL_RGBA;
        } glBindTexture(GL_TEXTURE_2D, ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // 如果是.png就GL_CLAMP_TO_EDGE
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // .jpg就GL_REPEAT
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    } stbi_image_free(data);
    return ID;
} bool cmp(std::pair<glm::vec3, float> &lhs, std::pair<glm::vec3, float> &rhs) {
    return lhs.second < rhs.second;
}
