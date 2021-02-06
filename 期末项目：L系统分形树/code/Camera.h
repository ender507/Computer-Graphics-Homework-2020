#pragma once
#define GLEW_STATIC
#include <GL\glew.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "common.h"

namespace Camera {

	enum Camera_Movement {
		FORWARD,
		BACKWARD, 
		LEFTS,
		RIGHTS,
		JUMP
	};
	/////Ìí¼Ó
	enum Camera_Setting {
		SPEED_CHANGE,
		FLY_OR_NOT,
		JUMPING
	};
	const GLfloat YAW = 0.0f;
	const GLfloat PITCH = 0.0f;
	const GLfloat SPEED = 30.0f;
	const GLfloat SENSITIVITY = 0.25f;
	const GLfloat ZOOM = 45.0f;

	class Camera {
	public:
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		GLfloat Yaw;
		GLfloat Pitch;
		GLuint record_count[2] = {0};
		glm::vec3 cameraState[5][4];                             //Ïà»ú×´Ì¬´æ´¢£¨Position£¬Front£©
		GLfloat cameraState2[5][2];                               //pitch,yaw
		GLfloat MovementSpeed;
		GLfloat MouseSensitivity;
		GLfloat Zoom;
		GLboolean Fly = true; /////
		GLboolean Jumping = false;
		GLfloat Jumptime = 0.0f;/////
		Camera::Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW,GLfloat pitch = PITCH);

		Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ,
			GLfloat yaw, GLfloat pitch);

		glm::mat4 GetViewMatrix();

		void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
		void ProcessKeyboard2(Camera_Setting set);
		void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrain_check = true);

		void ProcessMouseScroll(GLfloat yoffset);
		void record();
		void Show_record();
		void ChangeCamera();
		void getGraph();
	private:

		void updateCameraVectors();
	};

}