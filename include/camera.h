#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// defalut values
const GLfloat _pitch = 0.0f,
              _yaw = -90.0f,
              _fov = 60.0f,
              _speed = 2.5f,
              _sensitivity = 0.1f;

enum movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    glm::vec3 position,
              right,
              up,
              worldUp,
              front;
    GLfloat pitch,
            yaw;

    GLfloat fov,
            speed,
            sensitivity;
    Camera(const GLfloat &Pitch = _pitch, const GLfloat &Yaw = _yaw, const GLfloat &Fov = _fov, const GLfloat &Speed = _speed, const GLfloat &Sensitivity = _sensitivity); // default vectors constructor
    Camera(const GLfloat &posX, const GLfloat &posY, const GLfloat &posZ, const GLfloat &upX, const GLfloat &upY, const GLfloat &upZ, const GLfloat &Pitch = _pitch, const GLfloat &Yaw = _yaw); // given matrix
    glm::mat4 getViewMatrix();
    void processKeyboard(const movement &move, const GLfloat &deltaTime);
    void processMouseMovement(GLfloat offsetX, GLfloat offsetY);
    void processMouseScroll(const GLfloat &offsetY);
private:
    void updateVectors();
};

Camera::Camera(const GLfloat &Pitch, const GLfloat &Yaw, const GLfloat &Fov, const GLfloat &Speed, const GLfloat &Sensitivity) :
position(glm::vec3(0.0f, 0.0f, 5.0f)), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), front(glm::vec3(0.0f, 0.0f, -1.0f)) {
    pitch = Pitch, yaw = Yaw, fov = Fov, speed = Speed, sensitivity = Sensitivity;
    updateVectors();
} Camera::Camera(const GLfloat &posX, const GLfloat &posY, const GLfloat &posZ, const GLfloat &upX, const GLfloat &upY, const GLfloat &upZ, const GLfloat &Pitch, const GLfloat &Yaw) : front(glm::vec3(0.0f, 0.0f, -1.0f)), fov(_fov), speed(_speed), sensitivity(_sensitivity) {
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
