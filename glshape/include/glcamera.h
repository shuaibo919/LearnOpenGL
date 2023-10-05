#ifndef HSB_GL_CAMERA_H
#define HSB_GL_CAMERA_H

#include "glad/glad.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>

enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 2.5f;
constexpr float SENSITIVITY = 0.05f;
constexpr float ZOOM = 45.0f;

class GLBasicCamera
{
private:
    // camera Attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;
    // euler Angles
    float m_yaw;
    float m_pitch;
    // camera options
    float m_speed;
    float m_mouseSensitivity;
    float m_zoom;
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        m_right = glm::normalize(glm::cross(m_front, m_worldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }

public:
    GLBasicCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f)
        : m_position(position), m_worldUp(up), m_yaw(yaw), m_pitch(pitch), m_speed(SPEED), m_mouseSensitivity(SENSITIVITY), m_zoom(ZOOM)
    {
        updateCameraVectors();
    }
    glm::mat4 viewMatrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }
    glm::mat4 projectionMatrix(GLuint src_width, GLuint src_height)
    {
        return glm::perspective(glm::radians(m_zoom), (float)src_width / (float)src_height, 0.1f, 100.0f);
    }
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        float cameraSpeed = static_cast<float>(2.5 * deltaTime);
        if (direction == CameraMovement::FORWARD)
            m_position += cameraSpeed * m_front;
        if (direction == CameraMovement::BACKWARD)
            m_position -= cameraSpeed * m_front;
        if (direction == CameraMovement::LEFT)
            m_position -= m_right * cameraSpeed;
        if (direction == CameraMovement::RIGHT)
            m_position += m_right * cameraSpeed;
    }
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= m_mouseSensitivity;
        yoffset *= m_mouseSensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            if (m_pitch < -89.0f)
                m_pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset)
    {
        m_zoom -= (float)yoffset;
        if (m_zoom < 1.0f)
            m_zoom = 1.0f;
        if (m_zoom > 45.0f)
            m_zoom = 45.0f;
    }
};

#endif