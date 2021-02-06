#pragma once
#include "shader.h"
#include "texture.h"
#include <vector>
#include "common.h"
#include "camera.h"

class Skybox {
public:

	Skybox();

	~Skybox();

	void Draw(glm::mat4 view, glm::mat4 projection, Season season);		// 绘制渲染天空盒场景
	void SetSeason(Season new_season) { this->season = new_season; };   // 设置当前天空盒的场景
	void LoadSkyBox();	// 加载四个季节的天空盒
	Season GetSeason() { return this->season; };	// 获取天空盒的当前季节	

private:
	unsigned int skymap[4];			// 四季天空盒纹理编号
	unsigned int skyVAO, skyVBO;
	Season season = SPRING;			// 天空盒当前场景的季节，默认为春天
	Shader* shader;

	void loadCubeMap(vector<string> faces, int season);

};