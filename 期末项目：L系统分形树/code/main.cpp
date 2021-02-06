#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include "SceneController.h"



void key_callback(GLFWwindow* window,int key, int scancode,int action,int mode);
void do_movement(Scene& scene, Season& season);
void mouse_callback(GLFWwindow* window,double xpos,double ypos);
void scroll_callback(GLFWwindow* window,double xoffset,double yoffset);

//const GLfloat screenWidth = 1600;
//const GLfloat screenHeight = 1000;
Camera::Camera camera(glm::vec3(-200.0f,8.0f,-200.0f));
bool keys[1024];
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = 400,lastY = 300;
bool firstMouse = true;


int main(){
	// 配置窗口基本参数
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(screenWidth,screenHeight,"L-system",NULL,NULL);
	if(window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// 配置基本交互函数
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window,mouse_callback);
	glfwSetScrollCallback(window,scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK){
		std::cout << "Failed to initialize GLEW" << std::endl;	
		return -1;
	}
	glViewport(0,0,screenWidth,screenHeight);
	Season season = SPRING;
	// 创建当前场景
	Scene scene(glfwGetTime());
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// 游戏循环
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		// 依据移动位置来重定位视角
		do_movement(scene, season);
		// 渲染当前场景
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
		glm::mat4 projection(1.0f);
		glm::mat4 model(1.0f);
		glm::mat4 view = camera.GetViewMatrix();

		projection = glm::perspective(glm::radians(camera.Zoom), 
			screenWidth / screenHeight, 0.001f, 2000.0f);  /////鼠标滚轮缩放

		scene.render(model, view, projection, camera, deltaTime, glfwGetTime(),season);////里面设置平行光方向或scene新增一个根据时间修改lightmatrix的函数

		// 更新时间
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window,GL_TRUE);
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)    /////检测按键一次，开关相机飞行
		camera.ProcessKeyboard2(Camera::FLY_OR_NOT);
	if (key == GLFW_KEY_U && action == GLFW_PRESS)     /////检测按键一次，设置相机速度
		camera.ProcessKeyboard2(Camera::SPEED_CHANGE);
	if (key==GLFW_KEY_SPACE && action == GLFW_PRESS)  ///// 检测按键一次，设置相机跳跃功能
		camera.ProcessKeyboard2(Camera::JUMPING);
	if (key==GLFW_KEY_P && action == GLFW_PRESS)      ///// 检测按键一次，拍照
		camera.getGraph();
	if (key == GLFW_KEY_R && action == GLFW_PRESS)      ///// 检测按键一次，记录当前相机位置
		camera.record();
	if (key == GLFW_KEY_F && action == GLFW_PRESS)      ///// 检测按键一次，更换相机位置
		camera.ChangeCamera();

	if(action == GLFW_PRESS)
		keys[key] = true;
	else if(action == GLFW_RELEASE)
		keys[key] = false;
	
}

void do_movement(Scene& scene, Season& season) {
	//GLfloat cameraSpeed = 5.0f*deltaTime; /////需要修改
	if(keys[GLFW_KEY_W])
		camera.ProcessKeyboard(Camera::FORWARD,deltaTime);
	if(keys[GLFW_KEY_S])
		camera.ProcessKeyboard(Camera::BACKWARD,deltaTime);
	if(keys[GLFW_KEY_A])
		camera.ProcessKeyboard(Camera::LEFTS,deltaTime);
	if(keys[GLFW_KEY_D])
		camera.ProcessKeyboard(Camera::RIGHTS,deltaTime);
	if (camera.Jumping)                   ///跳跃
		camera.ProcessKeyboard(Camera::JUMP, deltaTime);

	//if (keys[GLFW_KEY_I])      ///// 按住显示相机组信息，暂未实现
	//	camera.Show_record();

	if (keys[GLFW_KEY_1])
		season = SPRING;
	if (keys[GLFW_KEY_2])
		season = SUMMER;
	if (keys[GLFW_KEY_3])
		season = FALL;
	if (keys[GLFW_KEY_4])
		season = WINTER;
	if (keys[GLFW_KEY_E]) {
		scene.treeGrowUp();
		keys[GLFW_KEY_E] = GL_FALSE;
	}
}

void mouse_callback(GLFWwindow* window,double xpos,double ypos){
	if(firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset,yoffset);
}

void scroll_callback(GLFWwindow* window,double xoffset,double yoffset){
	camera.ProcessMouseScroll(yoffset);
}
