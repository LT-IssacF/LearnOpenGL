#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// 窗口回调函数
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

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 0;
    }
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // 每当窗口调整大小的时候调用窗口回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 渲染循环
    while(!glfwWindowShouldClose(window)) {
        // 输入
        processInput(window);
        // 渲染指令
        glClear(GL_COLOR_BUFFER_BIT);
        // 检查触发事件、更新窗口动态并调用相应的回调函数
        glfwPollEvents();
        // 交换缓冲
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
