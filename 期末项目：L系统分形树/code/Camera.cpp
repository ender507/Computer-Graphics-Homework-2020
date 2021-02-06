#include "Camera.h"

namespace Camera {

	Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw,GLfloat pitch) 
		:Front(glm::vec3(-100.0f, -6.0f, -100.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY),
		Zoom(ZOOM) {
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

	Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
	}

	glm::mat4 Camera::GetViewMatrix() {
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}

	void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
		GLfloat velocity = this->MovementSpeed * deltaTime;
		if (this->Fly)
		{
			Jumping = false;
			if (direction == FORWARD)
				this->Position += this->Front * velocity;
			if (direction == BACKWARD)
				this->Position -= this->Front * velocity;
			if (direction == LEFTS)
				this->Position -= this->Right * velocity;
			if (direction == RIGHTS)
				this->Position += this->Right * velocity;
		}
		else
		{
			GLfloat temp = 0.0f;
			if (Jumping)
			{
				Jumptime += deltaTime;                                       //渲染时间不同，deltaTime不同，导致跳跃的帧数变化                
				temp = 10 *  (10 * Jumptime - 0.5 * 10 * Jumptime * Jumptime);//h = vt - 0.5gt2 .跳跃滞空时间约2秒 
				if (temp > 0)
					this->Position.y = 8.0 + temp;
				else
				{
					this->Position.y = 8.0f;
					Jumptime = 0.0f;
					Jumping = false;
				}
			}
			//temp = this->Position.y;
			glm::vec3 front = glm::normalize(glm::vec3(this->Front.x ,0,this->Front.z));
			glm::vec3 right = glm::normalize(glm::vec3(this->Right.x, 0, this->Right.z));
			if (direction == FORWARD)
				this->Position += front * velocity;
			if (direction == BACKWARD)
				this->Position -= front * velocity;
			if (direction == LEFTS)
				this->Position -= right * velocity;
			if (direction == RIGHTS)
				this->Position += right * velocity;
			//this->Position.y = temp;
		}
	}

	void Camera::ProcessKeyboard2(Camera_Setting set)
	{
		if (set == SPEED_CHANGE)                    //相机移动速度三档30，100，200
		{
			if (this->MovementSpeed == 30.0f)
				this->MovementSpeed = 100.0f;
			else if (this->MovementSpeed == 100.0f)
				this->MovementSpeed = 200.0f;
			else if (this->MovementSpeed == 200.0f)
				this->MovementSpeed = 30.0f;
		}
		if (set == FLY_OR_NOT)
		{
			if (this->Fly)
			{
				this->Fly = false;
				this->Position.y = 8.0f; ///高度初始化
			}
			else
				this->Fly = true;
		}
		if (set == JUMPING)
			this->Jumping = true;
	}

	void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrain_check) {
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw += xoffset;
		this->Pitch += yoffset;
        //防止俯仰角pitch为90度
		if (constrain_check) {
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}
		
		this->updateCameraVectors();
	}

	void Camera::ProcessMouseScroll(GLfloat yoffset) {
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

	void Camera::updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw))*cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw))*cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
	}
	void Camera::record(){
		record_count[0] %= 5;
		cameraState[record_count[0]][0] = this->Position;
		cameraState[record_count[0]][1] = this->Front;
		cameraState[record_count[0]][2] = this->Right;
		cameraState[record_count[0]][3] = this->Up;
		cameraState2[record_count[0]][0] = this->Pitch;
		cameraState2[record_count[0]][1] = this->Yaw;
		record_count[0]++;
		//std::cout << 1 << record_count[0] << std::endl;
	}
	void Camera::Show_record(){
		//屏幕显示record的相关信息（记录的所有相机位置position即可）需要用到freetype库显示字体
		//等待一个有能组员
		std::cout << "当前相机位置：（" << this->Position.x <<"，"<< this->Position.y << "，" << this->Position.z <<"）" <<std::endl;
	}
	void Camera::ChangeCamera(){
		if (record_count[0])     //避免读空
		{
			record_count[1] %= record_count[0] ;
			this->Position = cameraState[record_count[1]][0];
			this->Front = cameraState[record_count[1]][1];
			this->Right = cameraState[record_count[1]][2];
			this->Up = cameraState[record_count[1]][3];
			this->Pitch=cameraState2[record_count[0]][0] ;
			this->Yaw= cameraState2[record_count[0]][1] ;
			updateCameraVectors();
			record_count[1]++;
			//std::cout << 2 << record_count[1] << std::endl;
		}
	}
	void Camera::getGraph(){
		std::cout << "拍照" << std::endl;
		unsigned char* m_image = new unsigned char[screenWidth * screenHeight * 4];
		glReadPixels(0, 0, screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE,m_image);
		/*若觉得图片太大，可采取间隔取样，例子如下
		unsigned char* m_image2 = new unsigned char[screenWidth/2 * screenHeight/2 * 4];
		for(int i = 0 ; i < screenWidth/2 * screenHeight/2 -1;i++)
		   m_image2[i] = m_image[16*(int)(i/4) + i%4];
		stbi_write_png("./result.png",
			screenWidth/2,
			screenHeight/2,
			4,
			m_image2, screenWidth/2 * 4);
		*/
		stbi_flip_vertically_on_write(1);
		stbi_write_png("./result.png",
			screenWidth,
			screenHeight,
			4,
			m_image, screenWidth*4);
		delete m_image;
	}
}


