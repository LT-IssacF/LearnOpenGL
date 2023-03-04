#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader_s.h>
#include <stb_image.h>
#include <iostream>
#include <cmath>
/*
scaling matrix      translation matrix
  x  0  0  0           1  0  0  x
  0  y  0  0           0  1  0  y 
  0  0  z  0           0  0  1  z
  0  0  0  1           0  0  0  1

rotate matrix(x)   rotate matrix(y)   rotate matrix(z)
  1   0   0   0     cos  0  sin  0     cos -sin  0  0
  0  cos -sin 0      0   1   0   0     sin  cos  0  0
  0  sin  cos 0    -sin  0  cos  0      0    0   1  0
  0   0   0   1      0   0   0   1      0    0   0  1
*/
const GLuint _width = 800;
const GLuint _height = 600;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(_width, _height, "Textures", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress))) {
        std::cout << "Failed to load GLAD" << std::endl;
        return 0;
    }
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader myShader("../src/05_transformations/shader/vertexShaderSource.vs.glsl", "../src/05_transformations/shader/fragmentShaderSource.fs.glsl");

    GLfloat vertices[] = {
        //   positions            colors        texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    GLuint indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0); // first parameter, layout (location = 0), aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // layout (location = 1), aColor
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // layout (location = 2), aTexCoord
    glEnableVertexAttribArray(2);

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
        // Mipmap(多级渐远纹理): 在一系列的纹理图像中，后一个纹理图像的分辨率是前一个的二分之一
        // 但是在不同级别的多级渐远纹理之间会产生不真实的生硬边界，所以在两个不同的Mipmap之间也可以使用different filtering mode
        // 一个常见的错误是，将放大过滤的选项设置为Minmap的过滤选项之一，这样不会有任何效果
        // generate texture
        // .jpg是三通道
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
        // .png是四通道
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture2" << std::endl;
    } stbi_image_free(data);

    myShader.use();
    // two ways to set uniforms
    glUniform1i(myShader.getUniformLocation("texture1"), 0);
    myShader.setInt("texture2", 1);
    GLint index;
    glm::mat4 trans;

    while(!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        glClear(GL_COLOR_BUFFER_BIT);
        // draw
        // case 1. ----------------------------
        // glActiveTexture(GL_TEXTURE0); // 一个窗口内应用两张纹理，就激活两个纹理对象
        // glBindTexture(GL_TEXTURE_2D, texture1); // 激活与绑定尽量成对
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);
        // // .translate(matrix, coord), .rotate(matrix, angle, rotate axis), .scale(matrix, scale parameter)
        // trans = glm::mat4(1.0f);
        // trans = glm::rotate(trans, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        // trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
        // trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
        // glUniformMatrix4fv(myShader.getUniformLocation("transform"), 1, GL_FALSE, glm::value_ptr(trans));
        // glBindVertexArray(VAO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // /*
        // Explain: In real world, if we wanna this box just rotate at the corner, we need let it rotate first, then translate it to the corner.
        // But at OpenGL render pipeline, this process is reverse with above on.
        // For example, now this case I want the box rotate around the center of the window. In the real world, I need move it to the edge and then rotate itself, 
        // but at this program, I need to reverse these multiplication, they are rotate first translate second.
        // https://learnopengl.com/code_viewer.php?code=getting-started/transformations-exercise1
        // */

        // case 2.------------ 给两个不同的纹理以不同的变换
        // texture1: container
        index = 0;
        glUniform1i(myShader.getUniformLocation("index"), index);
        glActiveTexture(GL_TEXTURE0); // 激活第一张纹理
        glBindTexture(GL_TEXTURE_2D, texture1);
        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f)); // bottom right
        trans = glm::rotate(trans, static_cast<GLfloat>(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(myShader.getUniformLocation("transform"), 1, GL_FALSE, &trans[0][0]); // 更直接的也可以直接取数组地址
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // texture2: awesomeface
        index = 1;
        glUniform1i(myShader.getUniformLocation("index"), index);
        glActiveTexture(GL_TEXTURE1); // 激活第二张纹理
        glBindTexture(GL_TEXTURE_2D, texture2);
        GLfloat scaleParameter = static_cast<GLfloat>(sin(glfwGetTime()));
        trans = glm::mat4(1.0f); // 小心沿用上次循环数据，犯未初始化的的低级错误
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f)); // top left
        trans = glm::scale(trans, glm::vec3(scaleParameter, scaleParameter, scaleParameter));
        glUniformMatrix4fv(myShader.getUniformLocation("transform"), 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    myShader.deleteProgram();

    glfwTerminate();
    return 0;
}
