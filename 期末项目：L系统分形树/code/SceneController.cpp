#include "SceneController.h"
glm::vec3 lightPos(-100, 50, 10);
// 创建场景
Scene::Scene(float time)
{
	skybox = new Skybox;			// 天空盒
	floor = new Floor;				// 地面
	fractTree = new FractalSystem(2);	// 分形树
	modelShader = new Shader(std::string(ShaderPath + "Shaders/model.vs").c_str(),
		std::string(ShaderPath + "Shaders/model.fs").c_str());// 阴影

	glUseProgram(0);

	fractTree->process();			// 生成分形树
	
	//阴影部分
	GLfloat near_plane = 1.0f, far_plane = 900.0f; //因为我们使用的是一个所有光线都平行的定向光。出于这个原因，我们将为光源使用正交投影矩阵，透视图将没有任何变形
	GLfloat ws = 800.0f;
	glm::mat4 lightProjection = glm::ortho(-ws, ws, -ws, ws, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0)); //为了创建一个视图矩阵来变换每个物体，把它们变换到从光源视角可见的空间中，我们将使用glm::lookAt函数；这次从光源的位置看向场景中央。

	lightSpaceMatrix1 = lightProjection * lightView; //二者相结合为我们提供了一个光空间的变换矩阵，它将每个世界空间坐标变换到光源处所见到的那个空间

	glGenFramebuffers(1, &depthMapFBO); // 创建帧缓冲对象
	GenDepthTex();
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO); //把生成的深度纹理作为帧缓冲的深度缓冲
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthMap,0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "Error to get frame buffer" << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowShader = new Shader(string(ShaderPath + "Shaders/shadow.vs").c_str(), //顶点着色器将一个单独模型的一个顶点，使用lightSpaceMatrix变换到光空间中
								string(ShaderPath + "Shaders/shadow.fs").c_str()); //由于没有颜色缓冲，最后的片段不需要任何处理，使用一个空片段着色器
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

	GLfloat near_plane = 1.0f, far_plane = 900.0f; //因为我们使用的是一个所有光线都平行的定向光。出于这个原因，我们将为光源使用正交投影矩阵，透视图将没有任何变形
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

	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0)); //为了创建一个视图矩阵来变换每个物体，把它们变换到从光源视角可见的空间中，我们将使用glm::lookAt函数；这次从光源的位置看向场景中央。
	lightSpaceMatrix1 = lightProjection * lightView; //二者相结合为我们提供了一个光空间的变换矩阵，它将每个世界空间坐标变换到光源处所见到的那个空间
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


void Scene::GenDepthTex() { // 创建深度纹理
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