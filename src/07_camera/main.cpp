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
// 右手系坐标系统的z轴正方向是指向屏幕外的
Camera myCamera(0.0f, 0.0f, 5.0f, 0.0f, 1.0f, 0.0f);
GLfloat deltaTime = 0.0f, lastFrame = 0.0f;
GLfloat lastX = _width / 2.0f, lastY = _height / 2.0f;
bool firstMouse = true;
// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f), cameraRight = glm::vec3(1.0f, 0.0f, 0.0f), cameraUp = glm::vec3(0.0f, 1.0f, 0.0f), cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// GLfloat pitch = 0.0f, yaw = -90.0f, cameraSpeed = 2.5f, fov = 60.0f, sensitivity = 0.1f; // 俯仰角，偏航角

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double offsetX, double offsetY);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(_width, _height, "Camera", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return 0;
    } glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress))) {
        std::cout << "Failed to load GLAD" << std::endl;
        return 0;
    } glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader myShader("../src/07_camera/shader/vertexShaderSource.vs.glsl", "../src/07_camera/shader/fragmentShaderSource.fs.glsl");

    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    }; GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0); // first parameter, layout (location = 0), aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // layout (location = 1), aTexCoord
    glEnableVertexAttribArray(1);

    // gen texture obj
    GLuint texture1, texture2;
    glGenTextures(1, &texture1);
    // bind texture
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // assign edge color
    GLfloat borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // set filtering mode: use Nearest Neighbor Filtering when Minify, and Linear Filtering when Magnify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("../resource/texture/container.jpg", &width, &height, &nrChannels, 0);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture1" << std::endl;
    } stbi_image_free(data);

    // texture2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("../resource/texture/awesomeface.png", &width, &height, &nrChannels, 0);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture2" << std::endl;
    } stbi_image_free(data);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    myShader.use();
    glUniform1i(myShader.getUniformLocation("texture1"), 0);
    myShader.setInt("texture2", 1);    
    glEnable(GL_DEPTH_TEST);
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    glm::mat4 model, view, projection;
    glm::vec3 originPos = glm::vec3(0.0f, 0.0f, 0.0f), upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    GLfloat radius = 10.0f, camX, camZ;

    while(!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window);

        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // draw
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        myShader.use();

        view = myCamera.getViewMatrix();
        // view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(myShader.getUniformLocation("view"), 1, GL_FALSE, &view[0][0]);
        projection = glm::perspective(glm::radians(myCamera.fov), static_cast<GLfloat>(_width / _height), 0.1f, 100.0f);
        glUniformMatrix4fv(myShader.getUniformLocation("projection"), 1, GL_FALSE, &projection[0][0]);
        // the projection matrix rarely changes so it's often best practice to set it outside the main loop only once, but when we wanna change fov which can effect projection matrix, so this is special case

        glBindVertexArray(VAO);
        for(int i = 0; i < 10; i++) {
            model = glm::mat4(1.0f); // reset matrix
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(static_cast<GLfloat>(glfwGetTime()) * 10.0f * (i + 1.0f)), glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(myShader.getUniformLocation("model"), 1, GL_FALSE, &model[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    myShader.deleteProgram();

    glfwTerminate();
    return 0;
} // main

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
} void processInput(GLFWwindow *window) {
    // cameraSpeed = deltaTime * 2.5f;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else {
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            // cameraPos += cameraSpeed * cameraFront;
            myCamera.processKeyboard(FORWARD, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            // cameraPos -= cameraSpeed * cameraFront;
            myCamera.processKeyboard(BACKWARD, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            // cameraPos -= cameraSpeed * cameraRight;
            myCamera.processKeyboard(LEFT, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            // cameraPos += cameraSpeed * cameraRight;
            myCamera.processKeyboard(RIGHT, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            // cameraPos += 0.8f * cameraSpeed * cameraUp;
            myCamera.processKeyboard(UP, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            // cameraPos -= 0.8f * cameraSpeed * cameraUp;
            myCamera.processKeyboard(DOWN, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            myCamera.processKeyboard(LOOK_LEFT, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            myCamera.processKeyboard(LOOK_RIGHT, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { // key P
            myCamera.processKeyboard(LOOK_UP, deltaTime);
        } else if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { // key L
            myCamera.processKeyboard(LOOK_DOWN, deltaTime);
        }
    }
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos, lastY = ypos;
        firstMouse = false;
    } myCamera.processMouseMovement(xpos - lastX, lastY - ypos);
    lastX = xpos, lastY = ypos;
    // GLfloat offsetX = (xpos - lastX) * sensitivity, offsetY = (lastY - ypos) * sensitivity;
    // lastX = xpos, lastY = ypos;
    // pitch += offsetY, yaw += offsetX;
    // if(pitch > 89.0f) {
    //     pitch = 89.0f;
    // } else if(pitch < -89.0f) {
    //     pitch = -89.0f;
    // } glm::vec3 front;
    // front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    // front.y = sin(glm::radians(pitch));
    // front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    // cameraFront = glm::normalize(front);
}
void scroll_callback(GLFWwindow *window, double offsetX, double offsetY) {
    // if(fov >= 1.0f && fov <= 120.0f) {
    //     fov -= offsetY;
    // } else if(fov < 1.0f) {
    //     fov = 1.0f;
    // } else if(fov > 120.0f) {
    //     fov = 120.0f;
    // }
    myCamera.processMouseScroll(offsetY);
}
