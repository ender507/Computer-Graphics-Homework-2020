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

	void Draw(glm::mat4 view, glm::mat4 projection, Season season);		// ������Ⱦ��պг���
	void SetSeason(Season new_season) { this->season = new_season; };   // ���õ�ǰ��պеĳ���
	void LoadSkyBox();	// �����ĸ����ڵ���պ�
	Season GetSeason() { return this->season; };	// ��ȡ��պеĵ�ǰ����	

private:
	unsigned int skymap[4];			// �ļ���պ�������
	unsigned int skyVAO, skyVBO;
	Season season = SPRING;			// ��պе�ǰ�����ļ��ڣ�Ĭ��Ϊ����
	Shader* shader;

	void loadCubeMap(vector<string> faces, int season);

};