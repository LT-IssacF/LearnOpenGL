#ifndef CAMERA_H
#define CAMERA_H
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// defalut values
const GLfloat _pitch = 0.0f,
              _yaw = -90.0f,
              _fov = 45.0f,
              _speed = 2.5f,
              _sensitivity = 0.1f, // 当前类存在的一个问题就是，当光标已经移动到屏幕的边沿时，程序就无法再获得鼠标移动的参数了，也就无法再继续朝边沿的方向转动
              _keyboardSensitivity = 25.0f; // 但可以通过QE按键部分解决（一般fps游戏都让准心始终保持在屏幕正中央）

enum movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    LOOK_LEFT,
    LOOK_RIGHT,
    LOOK_UP,
    LOOK_DOWN
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
            sensitivity,
            keyboardSensitivity;
    Camera(const GLfloat &Pitch = _pitch, const GLfloat &Yaw = _yaw, const GLfloat &Fov = _fov, const GLfloat &Speed = _speed, const GLfloat &Sensitivity = _sensitivity); // default vectors constructor
    Camera(const GLfloat &posX, const GLfloat &posY, const GLfloat &posZ, const GLfloat &upX, const GLfloat &upY, const GLfloat &upZ, const GLfloat &Pitch = _pitch, const GLfloat &Yaw = _yaw); // given matrix
    glm::mat4 getViewMatrix();
    void processKeyboard(const movement &move, const GLfloat &deltaTime);
    void processMouseMovement(GLfloat offsetX, GLfloat offsetY);
    void processMouseScroll(const GLfloat &offsetY);
private:
    void updateVectors();
};
#endif
