#pragma once
#include "skybox.h"
#include "floor.h"
#include "FractalTreeSystem.h"
#include "Camera.h"


class Scene {
public:

	Scene(float time);

	~Scene();

	void treeGrowUp();

	void render(glm::mat4 model, glm::mat4& view, glm::mat4& projection,
		Camera::Camera &camera, float deltaTime, float timeflow, Season season);

private:

	Skybox *skybox;
	Floor *floor;
	FractalSystem *fractTree;
	Shader *modelShader;
	Shader *shadowShader;

	GLuint depthMapFBO, depthMap;
	glm::mat4 lightSpaceMatrix1;
	glm::mat4 lightSpaceMatrix2;


	void OutsideRender(glm::mat4 model, glm::mat4& view, glm::mat4& projection,
		Camera::Camera &camera, float deltaTime, float &timeflow, Season season);

	void OutsideRenderShadow(glm::mat4 model, Camera::Camera &camera, float deltaTime, float &timeflow, Season season);

	void GenDepthTex();
};