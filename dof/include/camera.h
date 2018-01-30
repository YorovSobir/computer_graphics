#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  100.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


class Camera {
public:
    glm::vec3 _position;
    glm::vec3 _front;
    glm::vec3 _up;
    glm::vec3 _right;
    glm::vec3 _worldUp;
    float _yaw;
    float _pitch;
    float _movementSpeed;
    float _mouseSensitivity;
    float _zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
            : _front(glm::vec3(0.0f, 0.0f, -1.0f))
            , _movementSpeed(SPEED)
            , _mouseSensitivity(SENSITIVITY)
            , _zoom(ZOOM)
            , _position(position)
            , _worldUp(up)
            , _yaw(yaw)
            , _pitch(pitch)
    {
        updateCameraVectors();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
            : _front(glm::vec3(0.0f, 0.0f, -1.0f))
            , _movementSpeed(SPEED)
            , _mouseSensitivity(SENSITIVITY)
            , _zoom(ZOOM)
            , _position(glm::vec3(posX, posY, posZ))
            , _worldUp(glm::vec3(upX, upY, upZ))
            , _pitch(pitch)
    {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(_position, _position + _front, _up);
    }

    void processKeyboard(CameraMovement direction, float deltaTime) {
        float velocity = _movementSpeed * deltaTime;
        switch (direction) {
            case FORWARD:
                _position += _front * velocity;
                break;
            case BACKWARD:
                _position -= _front * velocity;
                break;
            case LEFT:
                _position -= _right * velocity;
                break;
            case RIGHT:
                _position += _right * velocity;
                break;
            default:
                break;
        }
    }

    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= _mouseSensitivity;
        yoffset *= _mouseSensitivity;

        _yaw   += xoffset;
        _pitch += yoffset;

        if (constrainPitch) {
            if (_pitch > 89.0f) {
                _pitch = 89.0f;
            }
            if (_pitch < -89.0f) {
                _pitch = -89.0f;
            }
        }

        updateCameraVectors();
    }

    void processMouseScroll(float yoffset)
    {
        if (_zoom >= 1.0f && _zoom <= 45.0f)
            _zoom -= yoffset;
        if (_zoom <= 1.0f)
            _zoom = 1.0f;
        if (_zoom >= 45.0f)
            _zoom = 45.0f;
    }

private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = static_cast<float>(cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)));
        front.y = static_cast<float>(sin(glm::radians(_pitch)));
        front.z = static_cast<float>(sin(glm::radians(_yaw)) * cos(glm::radians(_pitch)));
        _front = glm::normalize(front);
        _right = glm::normalize(glm::cross(_front, _worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        _up    = glm::normalize(glm::cross(_right, _front));
    }
};
#endif
