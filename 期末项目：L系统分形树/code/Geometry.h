#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <vector>
using namespace std;

namespace Geometry {

	static glm::mat4 GetTranMat(glm::vec3 start, glm::vec3 end, glm::mat4 &nmats)
	{
		glm::mat4 trans(1.0f);
		trans = glm::translate(trans, start);
		glm::vec3 z = glm::vec3(0, 0, -1);
		glm::vec3 p = start - end;
		glm::vec3 t = glm::cross(z, p);
		float Pi = 3.1415926;
		float angle = acos(glm::dot(z, p) / glm::length(p));
		glm::mat4 rotas(1.0f);
		rotas = glm::rotate(rotas, angle, t);
		nmats = rotas;
		return trans*rotas;
	}

	static void Cylinder(glm::vec3 start, glm::vec3 end, float radius,
		vector<glm::vec3>&vertice, vector<glm::vec2>&texcoord)
	{
		//目标向量长度
		float len = sqrt(pow(start.x - end.x, 2) + pow(start.y - end.y, 2) + pow(start.z - end.z, 2));
		glm::mat4 nm(1.0f);
		glm::mat4 model = GetTranMat(start, end, nm);
		//段数
		unsigned int slice = 6;
		float delta = 360.0f / (float)(slice - 1.0);
		float td = 1.0f / (float)(slice - 1.0);
		int index = vertice.size();
		for (unsigned int x = 0; x < slice - 1; x++) {
			float angle = delta*x;
			float rc1 = radius*cos(glm::radians(angle));
			float rs1 = radius*sin(glm::radians(angle));
			float rc2 = radius*cos(glm::radians(angle + delta));
			float rs2 = radius*sin(glm::radians(angle + delta));

			glm::vec3 point1 = glm::vec3(rc1, rs1, len);
			glm::vec3 point2 = glm::vec3(rc1, rs1, 0.0);
			glm::vec3 point3 = glm::vec3(rc2, rs2, len);
			glm::vec3 point4 = glm::vec3(rc2, rs2, 0.0);

			vertice.push_back(point1); vertice.push_back(point2); vertice.push_back(point3);
			vertice.push_back(point3); vertice.push_back(point2); vertice.push_back(point4);
			texcoord.push_back(glm::vec2(x, 3.0f)); texcoord.push_back(glm::vec2(x, -3.0f));
			texcoord.push_back(glm::vec2(x + 1, 3.0f)); texcoord.push_back(glm::vec2(x + 1, 3.0f));
			texcoord.push_back(glm::vec2(x, -3.0f)); texcoord.push_back(glm::vec2(x + 1, -3.0f));
		}
		for (unsigned int y = index; y < vertice.size(); y++) {
			glm::vec4 record = model*glm::vec4(vertice[y], 1.0f);
			vertice[y] = glm::vec3(record.x, record.y, record.z);
		}
	}

	static glm::vec3 CalcNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
	{
		return glm::normalize(glm::cross(p2 - p1, p3 - p1));
	}

	// 生成圆柱（树干）
	static void CylinderMesh(float len, float radius,
		vector<glm::vec3>&vertice, vector<glm::vec2>&texcoord, vector<glm::vec3>&normal)
	{	// 每个圆柱用6个矩形表示
		unsigned int slice = 6;
		float delta = 360.0f / (float)(slice - 1.0);
		vector<glm::vec3> nrl;
		nrl.reserve(2 * (slice - 1));
		// 分别计算每个矩形的参数
		for (unsigned int x = 0; x < slice - 1; x++) {
			float angle = delta*x;
			float rc1 = radius*cos(glm::radians(angle));
			float rs1 = radius*sin(glm::radians(angle));
			float rc2 = radius*cos(glm::radians(angle + delta));
			float rs2 = radius*sin(glm::radians(angle + delta));
			//得到矩形的四个点的坐标
			glm::vec3 point1 = glm::vec3(rc1, rs1, len);
			glm::vec3 point2 = glm::vec3(rc1, rs1, 0.0);
			glm::vec3 point3 = glm::vec3(rc2, rs2, len);
			glm::vec3 point4 = glm::vec3(rc2, rs2, 0.0);
			// 记录坐标（矩形相当于2个三角形6个点）
			vertice.push_back(point1); vertice.push_back(point2); vertice.push_back(point3);
			vertice.push_back(point3); vertice.push_back(point2); vertice.push_back(point4);
			// 记录纹理坐标
			texcoord.push_back(glm::vec2(x, 5.0f)); texcoord.push_back(glm::vec2(x, -5.0f));
			texcoord.push_back(glm::vec2(x + 1, 5.0f)); texcoord.push_back(glm::vec2(x + 1, 5.0f));
			texcoord.push_back(glm::vec2(x, -5.0f)); texcoord.push_back(glm::vec2(x + 1, -5.0f));
			// 记录法向量
			nrl.push_back(CalcNormal(point1, point2, point3));
			nrl.push_back(CalcNormal(point3, point2, point4));
		}
		// 计算每个点的法向量（邻接三个面的法向量的均值）
		glm::vec3 tmp[12];
		tmp[0] = tmp[10] = glm::normalize(nrl[0] + nrl[8] + nrl[9]);
		tmp[1] = tmp[11] = glm::normalize(nrl[0] + nrl[1] + nrl[9]);
		for (int x = 2, y = 0; x <= 9; x++, y++) {
			tmp[x] = glm::normalize(nrl[y] + nrl[y + 1] + nrl[y + 2]);
		}
		normal.push_back(tmp[0]), normal.push_back(tmp[1]), normal.push_back(tmp[2]);
		normal.push_back(tmp[2]), normal.push_back(tmp[1]), normal.push_back(tmp[3]);
		normal.push_back(tmp[2]), normal.push_back(tmp[3]), normal.push_back(tmp[4]);
		normal.push_back(tmp[4]), normal.push_back(tmp[3]), normal.push_back(tmp[5]);
		normal.push_back(tmp[4]), normal.push_back(tmp[5]), normal.push_back(tmp[6]);
		normal.push_back(tmp[6]), normal.push_back(tmp[5]), normal.push_back(tmp[7]);
		normal.push_back(tmp[6]), normal.push_back(tmp[7]), normal.push_back(tmp[8]);
		normal.push_back(tmp[8]), normal.push_back(tmp[7]), normal.push_back(tmp[9]);
		normal.push_back(tmp[8]), normal.push_back(tmp[9]), normal.push_back(tmp[10]);
		normal.push_back(tmp[10]), normal.push_back(tmp[9]), normal.push_back(tmp[11]);
	}

	static glm::vec3 RotateX(glm::vec3 target, float angle) {
		angle = glm::radians(angle);
		glm::mat3 matrix(0.0f);
		matrix[0][0] = 1.0f;
		matrix[1][1] = cos(angle);
		matrix[1][2] = sin(angle);
		matrix[2][1] = -sin(angle);
		matrix[2][2] = cos(angle);
		glm::vec3 ret = matrix*target;
		return ret;
	}

	static glm::vec3 RotateY(glm::vec3 target, float angle) {
		angle = glm::radians(angle);
		glm::mat3 matrix(0.0f);
		matrix[0][0] = cos(angle);
		matrix[0][2] = -sin(angle);
		matrix[1][1] = 1.0f;
		matrix[2][0] = sin(angle);
		matrix[2][2] = cos(angle);
		glm::vec3 ret = matrix*target;
		return ret;
	}

	static glm::vec3 RotateZ(glm::vec3 target, float angle) {
		angle = glm::radians(angle);
		glm::mat3 matrix(0.0f);
		matrix[0][0] = cos(angle);
		matrix[0][1] = sin(angle);
		matrix[1][0] = -sin(angle);
		matrix[1][1] = cos(angle);
		matrix[2][2] = 1.0f;
		glm::vec3 ret = matrix*target;
		return ret;
	}
}