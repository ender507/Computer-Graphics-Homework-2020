#pragma once
#include "shader.h"
#include "texture.h"
#include "common.h"
#include "Camera.h"

const string FLOORPIC_DIR = "textures/floor/";

class Floor {
private:
	GLuint VBO, VAO;
	Shader *shader;
	CTexture floor_spring, floor_summer, floor_fall, floor_winter;

public:
	Floor();

	~Floor();

	void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, Camera::Camera& camera,
		GLuint &depthMap, glm::mat4 &lightSpace, Season season);

	void renderShadow(Shader *shadowShader, glm::mat4 model);

};