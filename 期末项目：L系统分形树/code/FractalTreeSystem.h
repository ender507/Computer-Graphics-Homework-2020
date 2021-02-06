#pragma once
#include "common.h"
#include "Grammar.h"
#include "Geometry.h"
#include <ctime>
#include "Camera.h"
#include <string>

#define TREE_NUMS 120
#define TREE_LEVEL 4
#define LEAF_WIDTH 0.1f

struct State
{
	glm::vec3 pos;
	glm::vec3 dir;
	float length;
	float radius;
	int level;
};

struct Trunk
{
	glm::vec3 start;
	glm::vec3 end;
	float radius;
	float length;
	int level;
	Trunk() {
		start = end = glm::vec3(0.0f);
		level = 1;
	}
};

struct Leaf
{
	glm::vec3 pos;
	glm::vec3 dir;
};

class FractalSystem
{
public:
	double dx, dy, dz;
	double length, lengthFactor;
	double radius, radiusFactor;
	State curState;
	Grammar grammar;
	GLuint trunkVAO, trunkVBO, leafsVAO, leafsVBO;
	Shader* trunkShader;
	Shader *leafsShader_spring, *leafsShader_summer, *leafsShader_fall;
	CTexture trunkTex, leafsTex;
	int numTrunks, numLeafs;
	

	void DrawInit();
	void LeafInit(vector<Leaf>leafs, vector<glm::vec3>&vertiice, vector<glm::vec2>&texcoord);
	void TrunkInit(vector<Trunk>trunks, vector<glm::vec3>&vertice,
		vector<glm::vec2>&texcoord, vector<glm::vec3>&normal);

	struct cylinderNode {
		vector<glm::vec3>vertice;
		vector<glm::vec2>texcoord;
		vector<glm::vec3>normal;
	};

	map<double, cylinderNode >cylinderTable;
	vector<glm::mat4> instanceMatrix;

	int level;
	vector<Trunk> trunks;
	vector<Leaf> leafs;
	FractalSystem(int lvl);
	~FractalSystem();
	void clearAll();
	void initGrammar(int levels);
	void generateFractal();
	void process();
	void render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, Camera::Camera& camera, GLuint &depthMap, glm::mat4 &lightSpace, Season season);
	void renderShadow(Shader *shadowShader, glm::mat4 model, Season season);
};