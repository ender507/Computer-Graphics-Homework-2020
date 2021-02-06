#include "FractalTreeSystem.h"

FractalSystem::FractalSystem(int lvl = 2)
{
	// 旋转角度
	dx = dz = 35.0f;
	dy = 30.0f;
	// 树干长度和半径
	length = 4.0;
	radius = 0.15f;
	// 长度半径衰减比例
	lengthFactor = 0.75;
	radiusFactor = 0.72;
	// 树干和树叶计数
	numTrunks = numLeafs = 0;
	// 树的深度
	level = lvl;
}

FractalSystem::~FractalSystem()
{
	clearAll();
	delete trunkShader;
	delete leafsShader_spring;
	delete leafsShader_summer;
	delete leafsShader_fall;
}

// 创建树的渲染参数
void FractalSystem::DrawInit()
{
	vector<glm::vec3>trunkVer;
	vector<glm::vec2>texcoord;
	vector<glm::vec3>normal;
	// 通过树干参数进行建模
	TrunkInit(trunks, trunkVer, texcoord, normal);

	numTrunks = trunkVer.size();
	glGenBuffers(1, &trunkVBO);
	glGenVertexArrays(1, &trunkVAO);
	glGenBuffers(1, &leafsVBO);
	glGenVertexArrays(1, &leafsVAO);
	trunkShader = new Shader(std::string(ShaderPath + "Shaders/trunk.vs").c_str(),
		std::string(ShaderPath + "Shaders/trunk.fs").c_str());
	leafsShader_spring = new Shader(std::string(ShaderPath + "Shaders/leafs.vs").c_str(),
		std::string(ShaderPath + "Shaders/leafs_spring.fs").c_str());
	leafsShader_summer = new Shader(std::string(ShaderPath + "Shaders/leafs.vs").c_str(),
		std::string(ShaderPath + "Shaders/leafs_summer.fs").c_str());
	leafsShader_fall = new Shader(std::string(ShaderPath + "Shaders/leafs.vs").c_str(),
		std::string(ShaderPath + "Shaders/leafs_fall.fs").c_str());
	trunkTex.loadTexture(ShaderPath + "textures/bark.png", true);
	leafsTex.loadTexture(ShaderPath + "textures/leaves.png", false);
	trunkShader->use();
	trunkShader->setInt("bark", 0);
	trunkShader->setInt("shadowMap", 1);
	trunkShader->setVec3("dirLight.direction", glm::normalize(lightPos));
	trunkShader->setVec3("dirLight.ambient", ambientLight);
	trunkShader->setVec3("dirLight.diffuse", diffuseLight);
	leafsShader_spring->use();
	//leafsShader_spring->setInt("leaf_spring", 0);
	leafsShader_summer->use();
	//leafsShader_summer->setInt("leaf_summer", 0);
	leafsShader_fall->use();
	//leafsShader_fall->setInt("leaf_fall", 0);
	//trunks
	int tVs = trunkVer.size() * sizeof(glm::vec3);
	int tTs = texcoord.size() * sizeof(glm::vec2);
	int tNs = normal.size() * sizeof(glm::vec3);
	glBindVertexArray(trunkVAO);
	glBindBuffer(GL_ARRAY_BUFFER, trunkVBO);
	glBufferData(GL_ARRAY_BUFFER, tVs + tTs + tNs, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, tVs, &trunkVer[0]);
	glBufferSubData(GL_ARRAY_BUFFER, tVs, tTs, &texcoord[0]);
	glBufferSubData(GL_ARRAY_BUFFER, tVs + tTs, tNs, &normal[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
		(GLvoid*)(trunkVer.size() * sizeof(glm::vec3)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
		(GLvoid*)(trunkVer.size() * sizeof(glm::vec3) + texcoord.size() * sizeof(glm::vec2)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//leafs
	vector<glm::vec3>leafVer;
	vector<glm::vec2>leafTex;
	LeafInit(leafs, leafVer, leafTex);
	numLeafs = leafVer.size();
	int lVs = leafVer.size() * sizeof(glm::vec3);
	//int lTs = leafTex.size() * sizeof(glm::vec2);
	glBindVertexArray(leafsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, leafsVBO);
	glBufferData(GL_ARRAY_BUFFER, lVs, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, lVs, &leafVer[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, lVs, lTs, &leafTex[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)(numLeafs * sizeof(glm::vec3)));
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glBindVertexArray(0);

}


void FractalSystem::render(glm::mat4 model, glm::mat4 view, glm::mat4 projection, Camera::Camera& camera, GLuint &depthMap, glm::mat4 &lightSpace, Season season)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//render trunks
	model = glm::translate(model, glm::vec3(-110, 0, -120));
	model = glm::scale(model, glm::vec3(3.0f));
	trunkShader->use();
	trunkShader->setMat4("model", model);
	trunkShader->setMat4("view", view);
	trunkShader->setMat4("projection", projection);
	trunkShader->setMat4("lightSpaceMatrix", lightSpace);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, trunkTex.textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glBindVertexArray(trunkVAO);
	glDrawArrays(GL_TRIANGLES, 0, numTrunks);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//render leafs
	if (season == WINTER) return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	Shader* leafsShader = nullptr;
	if (season == SPRING)
		leafsShader = leafsShader_spring;
	else if (season == SUMMER)
		leafsShader = leafsShader_summer;
	else
		leafsShader = leafsShader_fall;
	leafsShader->use();
	leafsShader->setMat4("model", model);
	leafsShader->setMat4("view", view);
	leafsShader->setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, leafsTex.textureID);
	glBindVertexArray(leafsVAO);
	glDrawArrays(GL_POINTS, 0, numLeafs);
	glBindVertexArray(0);

}
void FractalSystem::renderShadow(Shader* shadowShader, glm::mat4 model, Season season)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	//render trunks
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-110, 0, -120));
	model = glm::scale(model, glm::vec3(3.0f));
	shadowShader->setMat4("model", model);
	glBindVertexArray(trunkVAO);
	glDrawArrays(GL_TRIANGLES, 0, numTrunks);
	glBindVertexArray(0);
	//render leafs
	if (season != WINTER) {
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(leafsVAO);
		glDrawArrays(GL_POINTS, 0, numLeafs);
		glBindVertexArray(0);
	}
	glDisable(GL_CULL_FACE);
}


void FractalSystem::clearAll()
{
	grammar.clear();
	trunks.clear();
	leafs.clear();
	cylinderTable.clear();
	instanceMatrix.clear();
}

void FractalSystem::process()
{
	initGrammar(level);		// 通过词法生成字符串
	generateFractal();		// 通过字符串生成树的建模参数
	DrawInit();				// 通过树的建模参数设置渲染参数
}

void FractalSystem::initGrammar(int levels)
{
	grammar.setGrammarName("Test1");
	// 设置词法
	grammar.addGeneration('S', "F[^$X][*%X][&%X]");
	grammar.addGeneration('X', "F[^%D][&$D][/$D][*%D]");
	grammar.addGeneration('X', "F[&%D][*$D][/$D][^%D]");
	grammar.addGeneration('D', "F[^$X][*%FX][&%X]");
	// 起始字符串
	grammar.setStart("S");
	grammar.iterateFor(levels);
}

void FractalSystem::generateFractal()
{
	trunks.clear();
	leafs.clear();
	curState.pos = glm::vec3(0, 0, 0);
	curState.dir = glm::vec3(0, 1, 0);
	curState.length = length;
	curState.level = 1;
	curState.radius = radius;

	std::stack<State>stacks;

	// 依据字符串生成树的参数
	for (int i = 0; i < grammar.getResult().size(); i++) {
		char ch = grammar.getResult()[i];
		Trunk tmp;
		switch (ch) {
		case 'F': {		// 向前生长
			tmp.start = curState.pos;
			curState.pos += curState.dir*(float)curState.length;
			tmp.end = curState.pos;
			//tmp.length = curState.length;
			tmp.radius = curState.radius;
			tmp.level = curState.level;
			trunks.push_back(tmp);
			break;
		}
		case '$': {		// 绕Y轴旋转
			curState.dir = Geometry::RotateY(curState.dir, dy);
			break;
		}
		case '%': {		// 绕Y轴反向旋转
			curState.dir = Geometry::RotateY(curState.dir, -dy);
			break;
		}
		case '^': {
			curState.dir = Geometry::RotateX(curState.dir, dx);
			break;
		}
		case '&': {
			curState.dir = Geometry::RotateX(curState.dir, -dx);
			break;
		}
		case '*': {
			curState.dir = Geometry::RotateZ(curState.dir, dz);
			break;
		}
		case '/': {
			curState.dir = Geometry::RotateZ(curState.dir, -dz);
			break;
		}
		case '[': {		// 入栈，生成分支
			stacks.push(curState);
			curState.length *= lengthFactor;
			curState.radius *= radiusFactor;
			curState.level += 1;
			break;
		}
		case ']': {		// 出栈，生成叶子
			if (curState.level == grammar.getLevel()) {
				Trunk tm = trunks[trunks.size() - 1];
				Leaf rs;
				rs.dir = tm.end - tm.start;
				rs.pos = tm.end;
				leafs.push_back(rs);
			}
			curState = stacks.top();
			stacks.pop();
			break;
		}
		default:
			break;
		}
	}
}

void FractalSystem::LeafInit(vector<Leaf>leafs, vector<glm::vec3>&vertice, vector<glm::vec2>&texcoord)
{
	int len = leafs.size();
	vertice.reserve(len);
	for (int x = 0; x < len; x++) {
		leafs[x].dir = glm::normalize(leafs[x].dir);
		vertice.push_back(leafs[x].dir*LEAF_WIDTH + leafs[x].pos);
	}
}

void FractalSystem::TrunkInit(vector<Trunk>trunks, vector<glm::vec3>&vertice,
	vector<glm::vec2>&texcoord, vector<glm::vec3>&normal)
{
	double curRadius = radius;
	double curLength = length;

	// 对每一层的树干进行初始建模（原点往z轴正方向延伸）
	for (int i = 0; i < level + 1; i++) {
		cylinderNode record;
		Geometry::CylinderMesh(curLength, curRadius, record.vertice, record.texcoord, record.normal);
		cylinderTable[i + 1] = record;
		curLength *= lengthFactor;
		curRadius *= radiusFactor;
	}

	int len = trunks.size();
	vertice.reserve(30 * len);
	texcoord.reserve(30 * len);
	normal.reserve(30 * len);
	// 对每一个树干进行移动和旋转，到达实际位置
	for (int x = 0; x < len; x++) {
		glm::mat4 nmats(1.0f);
		glm::mat4 trans = Geometry::GetTranMat(trunks[x].start, trunks[x].end, nmats);
		int nums = cylinderTable[trunks[x].level].vertice.size();
		for (int y = 0; y < nums; y++) {
			glm::vec4 tmp = trans*glm::vec4(cylinderTable[trunks[x].level].vertice[y], 1.0f);
			vertice.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
			tmp = nmats*glm::vec4(cylinderTable[trunks[x].level].normal[y], 1.0f);
			normal.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
		}
		texcoord.insert(texcoord.end(), cylinderTable[trunks[x].level].texcoord.begin(),
			cylinderTable[trunks[x].level].texcoord.end());
	}
}
