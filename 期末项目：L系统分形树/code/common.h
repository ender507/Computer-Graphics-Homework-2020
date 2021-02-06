#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stack>
#include <string>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace std;

#ifndef PI
#define PI 3.1415926
#endif
#ifndef PI2
#define PI2 6.2831853
#endif

#define SHADOW_WIDTH 4096
#define SHADOW_HEIGHT 4096
enum Season { SPRING, SUMMER, FALL, WINTER };
const GLfloat screenWidth = 1600;
const GLfloat screenHeight = 1000;
extern glm::vec3 lightPos;     //ƽ�йⷴ�򣬸���ʱ�������������ճ�����Ч��
const glm::vec3 sunPos(900, 900, 900);     //
const glm::vec3 ambientLight(0.3f);        ///������ϵ��
const glm::vec3 diffuseLight(0.7f);        ////������ϵ��
const glm::vec3 specularLight(1.0f);       /////���淴��ϵ��
const GLint shininess = 32;                //���淴��ǿ��,ֵԽ�󣬸߹ⷶΧԽС
const std::string ShaderPath = "../res/";