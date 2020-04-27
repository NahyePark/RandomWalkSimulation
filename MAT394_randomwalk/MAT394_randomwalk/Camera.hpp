/* Start Header -------------------------------------------------------
File Name: Camera.hpp
Purpose: Prototype of Camera class
Language: MSVC C++
Platform: VS2019, Windows
Project: Random Walk Simulation
Author: Nahye Park
Creation date: 03/26/2020
End Header --------------------------------------------------------*/
#pragma once

#ifndef CAMERA_HPP
#define CAMERA_HPP

#if !defined(UNREFERENCED_PARAMETER)
#define UNREFERENCED_PARAMETER(P) (P)
#endif


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	LOOK_UP,
	LOOK_DOWN,
	LOOK_RIGHT,
	LOOK_LEFT,
};

const float YAW = -150.0f;
const float PITCH = -10.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;



class Camera
{
public:

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;

	float movementSpeed;
	float mouseSensitivity;
	float zoom;
	float aspect;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	// Constructor with vectors
	Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f),
		float _yaw = YAW, float _pitch = PITCH, float _aspect = 4.0f/3.0f);
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatirx();
	void ProcessKeyboard(Camera_Movement _direction, float _deltaTime);

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
	void updateCameraVectors();

};

void ProcessInput(Camera* camera, GLFWwindow* window, float deltaTime);
void MouseCallback(GLFWwindow* window, Camera* camera, double xpos, double ypos, bool& firstMouse, float& lastX, float& lastY);
void ScrollCallback(GLFWwindow* window, Camera* camera, double xoffset, double yoffset);


#endif