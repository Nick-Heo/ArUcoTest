#include "AxisRenderer.h"

using namespace std;

AxisRenderer::AxisRenderer()
{
}

AxisRenderer::~AxisRenderer()
{
}

void AxisRenderer::initialize(const string& _shader_folder_path)
{
	shader.init(_shader_folder_path+"/colorShader.vert", _shader_folder_path+"/colorShader.frag");

	axisVertex.push_back(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
	axisVertex.push_back(Eigen::Vector3f(0.2f, 0.0f, 0.0f));
	axisVertex.push_back(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
	axisVertex.push_back(Eigen::Vector3f(0.0f, 0.2f, 0.0f));
	axisVertex.push_back(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
	axisVertex.push_back(Eigen::Vector3f(0.0f, 0.0f, 0.2f));

	axisColor.push_back(Eigen::Vector3f(1.0f, 0.0f, 0.0f));
	axisColor.push_back(Eigen::Vector3f(1.0f, 0.0f, 0.0f));
	axisColor.push_back(Eigen::Vector3f(0.0f, 1.0f, 0.0f));
	axisColor.push_back(Eigen::Vector3f(0.0f, 1.0f, 0.0f));
	axisColor.push_back(Eigen::Vector3f(0.0f, 0.0f, 1.0f));
	axisColor.push_back(Eigen::Vector3f(0.0f, 0.0f, 1.0f));

	vertexHandle = glGetAttribLocation(shader.getShaderId(), "vertexModel");
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, axisVertex.size() * sizeof(float) * 3, &axisVertex[0], GL_STATIC_DRAW);

	colorHandle = glGetAttribLocation(shader.getShaderId(), "vertexColor");
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, axisColor.size() * sizeof(float) * 3, &axisColor[0], GL_STATIC_DRAW);
	
	MVPMatrixHandle = glGetUniformLocation(shader.getShaderId(), "MVPMatrix");

	modelMatrix.setIdentity();
	defaultLineWidth = 2.0f;

	shader.bind();
}

void AxisRenderer::onDraw(Eigen::Matrix4f vpMatrix)
{
	shader.bind();
	glEnable(GL_DEPTH_TEST);

	MVPMatrix = vpMatrix * modelMatrix;

	glUniformMatrix4fv(MVPMatrixHandle, 1, GL_FALSE, MVPMatrix.data());

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(vertexHandle, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(vertexHandle);

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(colorHandle, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(colorHandle);

	glLineWidth(defaultLineWidth);
	glDrawArrays(GL_LINES, 0, axisVertex.size());

	shader.unbind();
}

void AxisRenderer::setModelMatrix(Eigen::Matrix4f inputModelMatrix) {
	modelMatrix = inputModelMatrix;
}

void AxisRenderer::setLineWidth(float lineWidth) {
	defaultLineWidth = lineWidth;
}


void AxisRenderer::setColor(AXIS axis, float r, float g, float b) {
	int axisIdx = (int)axis;

	if (axisIdx == 3) {
		axisIdx = 6;
		for (int i = 0; i < axisIdx; i++) {
			axisColor[i].x() = r;
			axisColor[i].y() = g;
			axisColor[i].z() = b;
		}
	}
	else {
		axisIdx = axisIdx * 2;
		axisColor[axisIdx].x() = r;
		axisColor[axisIdx].y() = g;
		axisColor[axisIdx].z() = b;
		axisColor[axisIdx + 1].x() = r;
		axisColor[axisIdx + 1].y() = g;
		axisColor[axisIdx + 1].z() = b;
	}

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, axisColor.size() * sizeof(float) * 3, &axisColor[0], GL_STATIC_DRAW);
}