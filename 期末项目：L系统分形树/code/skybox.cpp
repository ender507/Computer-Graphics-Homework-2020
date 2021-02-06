#include "skybox.h"


Skybox::Skybox() {
	float skyVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	this->LoadSkyBox();
	glGenVertexArrays(1, &skyVAO);
	glGenBuffers(1, &skyVBO);
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), &skyVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	shader = new Shader(string(ShaderPath + "Shaders/skybox.vs").c_str(),
		string(ShaderPath + "Shaders/skybox.fs").c_str());
	shader->setInt("skybox", 0);
}

Skybox::~Skybox() {
	glDeleteVertexArrays(1, &skyVAO);
	glDeleteBuffers(1, &skyVBO);
	delete shader;
}

void Skybox::Draw(glm::mat4 view, glm::mat4 projection, Season season) {
	this->season = season;
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	shader->use();
	shader->setMat4("view", glm::mat4(glm::mat3(view)));
	shader->setMat4("projection", projection);
	glBindVertexArray(skyVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skymap[season]);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
}

void Skybox::loadCubeMap(vector<string> faces, int season) {
	glGenTextures(1, &(skymap[season]));
	glActiveTexture(GL_TEXTURE0);
	int width, height, nrComponents;
	unsigned char* image;
	glBindTexture(GL_TEXTURE_CUBE_MAP, skymap[season]);
	for (GLuint i = 0; i < faces.size(); i++) {
		image = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		GLenum format;
		if (nrComponents == 1)format = GL_RED;
		else if (nrComponents == 3)format = GL_RGB;
		else if (nrComponents == 4)format = GL_RGBA;
		if (image) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
				width, height, 0, format, GL_UNSIGNED_BYTE, image);
		}
		else {
			std::cout << "Cube texture failed to load at path: " << faces[i] << std::endl;
		}
		stbi_image_free(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::LoadSkyBox() {
	vector<string> faces;
	faces.reserve(6);
	// 载入春天的天空盒图案
	faces.push_back(ShaderPath + "textures/skybox/spring/cq.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/spring/ch.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/spring/cs.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/spring/cd.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/spring/cz.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/spring/cy.jpg");		
	loadCubeMap(faces, 0);
	faces.clear();
	// 夏
	faces.push_back(ShaderPath + "textures/skybox/summer/xq.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/summer/xh.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/summer/xs.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/summer/xd.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/summer/xz.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/summer/xy.jpg");		
	loadCubeMap(faces, 1);
	faces.clear();
	// 秋
	faces.push_back(ShaderPath + "textures/skybox/autumn/qq.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/autumn/qh.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/autumn/qs.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/autumn/qd.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/autumn/qz.jpg");		
	faces.push_back(ShaderPath + "textures/skybox/autumn/qy.jpg");		
	loadCubeMap(faces, 2);
	faces.clear();
	// 冬季
	faces.push_back(ShaderPath + "textures/skybox/winter/dq.jpg");	
	faces.push_back(ShaderPath + "textures/skybox/winter/dh.jpg");	
	faces.push_back(ShaderPath + "textures/skybox/winter/ds.jpg");	
	faces.push_back(ShaderPath + "textures/skybox/winter/dd.jpg");	
	faces.push_back(ShaderPath + "textures/skybox/winter/dz.jpg");	
	faces.push_back(ShaderPath + "textures/skybox/winter/dy.jpg");	
	loadCubeMap(faces, 3);
}