
/* Start Header -------------------------------------------------------
File Name: Camera.cpp
Purpose: Move camera with user input
Language: C++
Platform: MSVC2019 window
Project: Random Walk Simulation
Author: Nahye Park
Creation date: 03/26/2020
End Header --------------------------------------------------------*/
#include "GLFW\glfw3.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.hpp"

Camera::Camera(glm::vec3 _position, glm::vec3 _up,
	float _yaw, float _pitch, float _aspect)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	
	position = _position;
	worldUp = _up;
	yaw = _yaw;
	pitch = _pitch;
	aspect = _aspect;
	updateCameraVectors();

	
}

glm::mat4 Camera::GetViewMatrix()
{
	ViewMatrix = glm::lookAt(position, position + front, up);
	return ViewMatrix;
}

glm::mat4 Camera::GetProjectionMatirx()
{
	return ProjectionMatrix;
}


void Camera::ProcessKeyboard(Camera_Movement _direction, float _deltaTime)
{
	float velocity = movementSpeed * _deltaTime;

	if (_direction == FORWARD) {
		position += front * velocity * 2.f;
	}
	if (_direction == BACKWARD) {
		position -= front * velocity * 2.f;
	}
	if (_direction == LEFT) {
		position -= right * velocity * 2.f;
	}
	if (_direction == RIGHT) {
		position += right * velocity * 2.f;
	}
	if (_direction == LOOK_UP) {
		pitch += 10 * velocity;
	}
	if (_direction == LOOK_DOWN) {
		pitch -= 10 * velocity;
	}
	if (_direction == LOOK_RIGHT) {
		yaw += 15 * velocity;
	}
	if (_direction == LOOK_LEFT) {
		yaw -= 15 * velocity;
	}
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	updateCameraVectors();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= yoffset;
	if (zoom <= 1.0f)
		zoom = 1.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	glm::vec3 _front;
	_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	_front.y = sin(glm::radians(pitch));
	_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(_front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	ProjectionMatrix = glm::perspective(glm::radians(ZOOM), aspect, 0.1f, 1000.0f);
}

void ProcessInput(Camera* camera, GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera->ProcessKeyboard(LOOK_RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera->ProcessKeyboard(LOOK_LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera->ProcessKeyboard(LOOK_UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera->ProcessKeyboard(LOOK_DOWN, deltaTime);
}
void MouseCallback(GLFWwindow* window, Camera* camera, double xpos, double ypos, bool& firstMouse, float& lastX, float& lastY)
{
	UNREFERENCED_PARAMETER(window);
	float f_xpos = float(xpos);
	float f_ypos = float(ypos);

	if (firstMouse)
	{
		lastX = f_xpos;
		lastY = f_ypos;
		firstMouse = false;
	}

	float xoffset = f_xpos - lastX;
	float yoffset = lastY - f_ypos;

	lastX = f_xpos;
	lastY = f_ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, Camera* camera, double xoffset, double yoffset)
{
	UNREFERENCED_PARAMETER(window);
	UNREFERENCED_PARAMETER(xoffset);
	camera->ProcessMouseScroll(float(yoffset));
}