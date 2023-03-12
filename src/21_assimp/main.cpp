#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <shader.h>
#include <stb_image.h>
#include <camera.h>
#include <model.h>
#include <iostream>
#define _POINT_LIGHTS 4

const unsigned int _width = 1680, _height = 1050;
Camera camera(0.0f, 10.0f, 20.0f, 0.0f, 1.0f, 0.0f);
float lastX = _width / 2.0f, lastY = _height / 2.0f, deltaTime = 0.0f, lastFrame = 0.0f;
bool firstMouse = true;
const char *glsl_version = "#version 330";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(_width, _height, "Nanosuit", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    } glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    } glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(false);
    // 创建imgui上下文
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImVec4 clearColor = ImVec4(0.05f, 0.05f, 0.05f, 1.0f);
    Shader lightShader("./src/21_assimp/shader/light.vs.glsl", "./src/21_assimp/shader/light.fs.glsl"); // 光源渲染
    GLfloat vertices[] = {
        //    position
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    }; GLuint lightVAO, VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // GL_ARRAY_BUFFER是这块缓冲的类型
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &lightVAO); // 光源用不上法向量，就可以直接无视法向量数据
    glBindVertexArray(lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glm::vec3 pointLightPositions[_POINT_LIGHTS] = {
        glm::vec3( 1.0f,  8.0f,  5.0f),
        glm::vec3( 0.0f,  7.5f, -5.0f),
        glm::vec3( 5.0f, 15.0f,  0.0f),
        glm::vec3(-5.0f,  12.0f, 4.0f)
    }; glm::vec3 pointLightColors[_POINT_LIGHTS] = {
        glm::vec3(1.0f,  1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f)
    }; 

    Shader shader("./src/21_assimp/shader/model_loading.vs.glsl", "./src/21_assimp/shader/model_loading.fs.glsl");
    // load models
    // -----------
    Model nanosuit("C:/Users/IssacF/Documents/Visual Studio Code/LearnOpenGL/resource/model/nanosuit/nanosuit.obj");
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glm::mat4 model, view, projection;
    while(!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("shader console");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::ColorEdit3("clear color", (float*)&clearColor); // tips: 形参需要一个float[]即float*，那么就可以取clearColor的地址再转成float*
        ImGui::End();
        // render
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setFloat("material.shininess", 32.0f);
        shader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        shader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
        shader.setVec3("dirLight.specular", 0.9f, 0.9f, 0.9f);
        shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        for(int i = 0; i < _POINT_LIGHTS; i++) {
            std::string index = "poiLights[" + std::to_string(i) + "].";
            shader.setVec3(index + "position", pointLightPositions[i]);
            shader.setVec3(index + "ambient", pointLightColors[i] * 0.2f);
            shader.setVec3(index + "diffuse", pointLightColors[i] * 1.0f);
            shader.setVec3(index + "specular", pointLightColors[i] * 1.2f);
            shader.setFloat(index + "constant", 1.0f);
            shader.setFloat(index + "linear", 0.045f);
            shader.setFloat(index + "quadratic", 0.0075f);
        } shader.setVec3("viewPos", camera.position);
        shader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f); 
        shader.setVec3("spotLight.diffuse", 0.9f, 0.9f, 0.9f);
        shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("spotLight.constant", 1.0f);
        shader.setFloat("spotLight.linear", 0.09f);
        shader.setFloat("spotLight.quadratic", 0.032f);
        shader.setVec3("spotLight.direction", camera.front);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(18.0f)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(23.0f)));
        shader.setVec3("viewPos", camera.position);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.fov), (float)_width / (float)_height, 0.1f, 100.0f);
        view = camera.getViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        shader.setMat4("model", model);
        nanosuit.Draw(shader);

         lightShader.use();
         glBindVertexArray(lightVAO); // 再使用光源的解释规则
         for(int i = 0; i < _POINT_LIGHTS; i++) {
             lightShader.setMat4("projection", projection);
             lightShader.setMat4("view", view);
             model = glm::mat4(1.0f);
             model = glm::translate(model, pointLightPositions[i]);
             model = glm::scale(model, glm::vec3(0.2f));
             lightShader.setMat4("model", model);
             lightShader.setVec3("lightColor", pointLightColors[i]);
             lightShader.setFloat("brightness", 0.1f);
             glDrawArrays(GL_TRIANGLES, 0, 36);
         }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    } shader.deleteProgram();
    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
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
} void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
} void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos, lastY = ypos;
        firstMouse = false;
    } camera.processMouseMovement(xpos - lastX, lastY - ypos);
    lastX = xpos, lastY = ypos;
} void scroll_callback(GLFWwindow* window, double offsetX, double offsetY) {
    camera.processMouseScroll(offsetY);
}
