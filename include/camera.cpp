#include "camera.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const GLfloat &Pitch, const GLfloat &Yaw, const GLfloat &Fov, const GLfloat &Speed, const GLfloat &Sensitivity) :
position(glm::vec3(0.0f, 0.0f, 5.0f)), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)) {
    pitch = Pitch, yaw = Yaw, fov = Fov, speed = Speed, sensitivity = Sensitivity, keyboardSensitivity = _keyboardSensitivity;
    updateVectors();
} Camera::Camera(const GLfloat &posX, const GLfloat &posY, const GLfloat &posZ, const GLfloat &upX, const GLfloat &upY, const GLfloat &upZ, const GLfloat &Pitch, const GLfloat &Yaw) : front(glm::vec3(0.0f, 0.0f, -1.0f)), fov(_fov), speed(_speed), sensitivity(_sensitivity), keyboardSensitivity(_keyboardSensitivity) {
    position = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    pitch = Pitch, yaw = Yaw;
    updateVectors();
} glm::mat4 Camera::getViewMatrix() { // 创建一个看着给定目标的观察矩阵
    return glm::lookAt(position, position + front, up); // lookAt(位置， 目标，上向量)
} void Camera::processKeyboard(const movement &move, const GLfloat &deltaTime) {
    GLfloat velocity = deltaTime * speed;
    if(move == FORWARD) {
        position += front * velocity;
    } else if(move == BACKWARD) {
        position -= front * velocity;
    } else if(move == LEFT) {
        position -= right * velocity;
    } else if(move == RIGHT) {
        position += right * velocity;
    } else if(move == UP) {
        position += up * velocity * 0.8f;
    } else if(move == DOWN) {
        position -= up * velocity * 0.8f;
    } else if(move == LOOK_LEFT) {
        processMouseMovement(-velocity * keyboardSensitivity / sensitivity, 0.0f);
        updateVectors();
    } else if(move == LOOK_RIGHT) {
        processMouseMovement(velocity * keyboardSensitivity / sensitivity, 0.0f);
        updateVectors();
    } else if(move == LOOK_UP) {
        processMouseMovement(0.0f, velocity * keyboardSensitivity / sensitivity);
        updateVectors();
    } else if(move == LOOK_DOWN) {
        processMouseMovement(0.0f, -velocity * keyboardSensitivity / sensitivity);
        updateVectors();
    } // 暂时不会用类实现多键组合输入
} void Camera::processMouseMovement(GLfloat offsetX, GLfloat offsetY) {
    offsetX *= sensitivity, offsetY *= sensitivity; // 根据鼠标移动的偏移量计算俯仰角和偏航角
    pitch += offsetY, yaw += offsetX;
    if(pitch > 89.0f) { // 控制最大俯仰角
        pitch = 89.0f;
    } else if(pitch < -89.0f) {
        pitch = -89.0;
    } updateVectors();
} void Camera::processMouseScroll(const GLfloat &offsetY) {
    fov -= offsetY;
    if(fov < 1.0f) {
        fov = 1.0f;
    } else if(fov > 120.0f) {
        fov = 120.0f;
    }
} void Camera::updateVectors() {
    glm::vec3 Front;
    Front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw)); // x轴的偏移由俯仰角和偏航角的余弦值决定
    Front.y = sin(glm::radians(pitch)); // y轴的偏移量仅由俯仰角的正弦值决定
    Front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw)); // z轴的偏移量分别由俯仰角的余弦值和偏航角的正弦值决定
    front = glm::normalize(Front);
    right = glm::normalize(glm::cross(front, worldUp)); // 叉乘
    up = glm::normalize(glm::cross(right, front));
}
