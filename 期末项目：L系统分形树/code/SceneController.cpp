#include "SceneController.h"
glm::vec3 lightPos(-100, 50, 10);
// ��������
Scene::Scene(float time)
{
	skybox = new Skybox;			// ��պ�
	floor = new Floor;				// ����
	fractTree = new FractalSystem(2);	// ������
	modelShader = new Shader(std::string(ShaderPath + "Shaders/model.vs").c_str(),
		std::string(ShaderPath + "Shaders/model.fs").c_str());// ��Ӱ

	glUseProgram(0);

	fractTree->process();			// ���ɷ�����
	
	//��Ӱ����
	GLfloat near_plane = 1.0f, far_plane = 900.0f; //��Ϊ����ʹ�õ���һ�����й��߶�ƽ�еĶ���⡣�������ԭ�����ǽ�Ϊ��Դʹ������ͶӰ����͸��ͼ��û���κα���
	GLfloat ws = 800.0f;
	glm::mat4 lightProjection = glm::ortho(-ws, ws, -ws, ws, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0)); //Ϊ�˴���һ����ͼ�������任ÿ�����壬�����Ǳ任���ӹ�Դ�ӽǿɼ��Ŀռ��У����ǽ�ʹ��glm::lookAt��������δӹ�Դ��λ�ÿ��򳡾����롣

	lightSpaceMatrix1 = lightProjection * lightView; //��������Ϊ�����ṩ��һ����ռ�ı任��������ÿ������ռ�����任����Դ�����������Ǹ��ռ�

	glGenFramebuffers(1, &depthMapFBO); // ����֡�������
	GenDepthTex();
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); //�����ɵ����������Ϊ֡�������Ȼ���
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthMap,0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Error to get frame buffer" << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowShader = new Shader(string(ShaderPath + "Shaders/shadow.vs").c_str(), //������ɫ����һ������ģ�͵�һ�����㣬ʹ��lightSpaceMatrix�任����ռ���
								string(ShaderPath + "Shaders/shadow.fs").c_str()); //����û����ɫ���壬����Ƭ�β���Ҫ�κδ���ʹ��һ����Ƭ����ɫ��
	shadowShader->use();
	shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix1);

	glm::mat4 biasMatrix{
		0.5,0.0,0.0,0.0,
		0.0,0.5,0.0,0.0,
		0.0,0.0,0.5,0.0,
		0.5,0.5,0.5,1.0
	};
	lightSpaceMatrix2 = biasMatrix * lightSpaceMatrix1;
	modelShader->use();
	modelShader->setInt("texture_diffuse1", 0);
	modelShader->setVec3("dirLight.direction", glm::normalize(lightPos));
	modelShader->setVec3("dirLight.ambient", ambientLight);
	modelShader->setVec3("dirLight.diffuse", diffuseLight);
	modelShader->setMat4("lightSpaceMatrix", lightSpaceMatrix2);
}

Scene::~Scene()
{
	delete skybox;
	delete fractTree;
	delete shadowShader;
}

void Scene::treeGrowUp() {
	int lvl = fractTree->level;
	double len = fractTree->length;
	double rad = fractTree->radius;
	if (lvl == 8)return;
	delete fractTree;
	fractTree = new FractalSystem(lvl + 1);
	fractTree->length = len * 1.1;
	fractTree->radius = rad * 1.1;
	fractTree->process();
}

void Scene::render(glm::mat4 model, glm::mat4 &view, glm::mat4 &projection,
	Camera::Camera &camera, float deltaTime, float timeflow, Season season)
{
	OutsideRender(model, view, projection, camera, deltaTime, timeflow, season);
}

void Scene::OutsideRender(glm::mat4 model, glm::mat4 &view, glm::mat4 &projection,
	Camera::Camera &camera, float deltaTime, float &timeflow, Season season)
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	OutsideRenderShadow(model, camera, deltaTime, timeflow, season);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat near_plane = 1.0f, far_plane = 900.0f; //��Ϊ����ʹ�õ���һ�����й��߶�ƽ�еĶ���⡣�������ԭ�����ǽ�Ϊ��Դʹ������ͶӰ����͸��ͼ��û���κα���
	GLfloat ws = 800.0f;
	glm::mat4 lightProjection = glm::ortho(-ws, ws, -ws, ws, near_plane, far_plane);
	if (season == SPRING) {
		lightPos.x = -100;
		lightPos.y = 50;
		lightPos.z = 10;
	}
	else if(season == SUMMER) {
		lightPos.x = -130;
		lightPos.y = 50;
		lightPos.z = 130;
	}
	else if(season == FALL) {
		lightPos.x = 50;
		lightPos.y = 100;
		lightPos.z = -500;
	}
	else {
		lightPos.x = -300;
		lightPos.y = 500;
		lightPos.z = -500;
	}

	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0)); //Ϊ�˴���һ����ͼ�������任ÿ�����壬�����Ǳ任���ӹ�Դ�ӽǿɼ��Ŀռ��У����ǽ�ʹ��glm::lookAt��������δӹ�Դ��λ�ÿ��򳡾����롣
	lightSpaceMatrix1 = lightProjection * lightView; //��������Ϊ�����ṩ��һ����ռ�ı任��������ÿ������ռ�����任����Դ�����������Ǹ��ռ�
	glm::mat4 biasMatrix{
		0.5,0.0,0.0,0.0,
		0.0,0.5,0.0,0.0,
		0.0,0.0,0.5,0.0,
		0.5,0.5,0.5,1.0
	};
	lightSpaceMatrix2 = biasMatrix * lightSpaceMatrix1;

	shadowShader->use();
	shadowShader->setMat4("lightSpaceMatrix", lightSpaceMatrix1);
	modelShader->use();
	modelShader->setInt("texture_diffuse1", 0);
	modelShader->setVec3("dirLight.direction", glm::normalize(lightPos));
	modelShader->setVec3("dirLight.ambient", ambientLight);
	modelShader->setVec3("dirLight.diffuse", diffuseLight);
	modelShader->setMat4("lightSpaceMatrix", lightSpaceMatrix2);
	skybox->Draw(view, projection, season);
	fractTree->render(model, view, projection, camera, depthMap, lightSpaceMatrix2, season);
	floor->render(model, view, projection, camera, depthMap, lightSpaceMatrix2, season);

	glUseProgram(0);

}

void Scene::OutsideRenderShadow(glm::mat4 model, Camera::Camera &camera, float deltaTime, float &timeflow, Season season)
{
	shadowShader->use();
	// model = glm::mat4(1.0f);
	shadowShader->setMat4("model", model);
	fractTree->renderShadow(shadowShader, model, season);
}


void Scene::GenDepthTex() { // �����������
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	GLfloat borderColor[] = { 1.0,1.0,1.0,1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);
}