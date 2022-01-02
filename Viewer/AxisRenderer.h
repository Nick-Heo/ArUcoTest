#pragma once

#include <vector>
#include <string>

#include <Eigen/Eigen>

#include "GL/glew.h"
#include "GL/freeglut.h"

#include "shader/Shader.h"

enum AXIS {
	X = 0,
	Y,
	Z,
	ALL
};

class AxisRenderer
{
public:
	AxisRenderer();
	~AxisRenderer();

	void initialize(const std::string& shaderFolderPath);
	void onDraw(Eigen::Matrix4f vpMatrix);

	void setModelMatrix(Eigen::Matrix4f modelMatrix);
	void setLineWidth(float lineWIdth);
	void setColor(AXIS axis, float r, float g, float b);

protected:
	Shader shader;

	GLuint MVPMatrixHandle;
	GLuint vertexHandle;
	GLuint vertexBuffer;
	GLuint colorHandle;
	GLuint colorBuffer;

	Eigen::Matrix4f MVPMatrix;
	Eigen::Matrix4f modelMatrix;

	float defaultLineWidth;

	std::vector<Eigen::Vector3f> axisVertex;
	std::vector<Eigen::Vector3f> axisColor;
};
