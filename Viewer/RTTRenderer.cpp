#include "RTTRenderer.h"

#include "GLMatrix.hpp"

using namespace std;

RTTRenderer::RTTRenderer()
{
}

RTTRenderer::~RTTRenderer()
{
}

void RTTRenderer::initialize(const string& _shader_folder_path)
{
	float vertex[12] =
	{ -0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f };

	unsigned char color[12] =
	{ 
		255, 0, 0,
		0, 255, 0,
		0, 0, 255,
		255, 255, 255 
	};

	float uv[8] =
	{ 0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f };

	screenWidth = 1024;
	screenHeight = 1024;

	shader.init(_shader_folder_path+"/texturedQuadShader.vert", _shader_folder_path+"/texturedQuadShader.frag");

	vertexHandle = glGetAttribLocation(shader.getShaderId(), "vertexModel");
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertex, GL_STATIC_DRAW);

	uvHandle = glGetAttribLocation(shader.getShaderId(), "vertexUV");
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), uv, GL_STATIC_DRAW);
	
	MVPMatrixHandle = glGetUniformLocation(shader.getShaderId(), "MVPMatrix");

	textureHandle = glGetUniformLocation(shader.getShaderId(), "texture");

	shader.bind();

	rtt.initialize(_shader_folder_path);

	Eigen::Matrix4f p = GLMatrix::ortho(-3, 3, -3, 3, 0.1f, 1000.f);
	Eigen::Matrix4f v = GLMatrix::lookAt(Eigen::Vector3f(0.0, 0.0, 100), Eigen::Vector3f(0.0, 0.0, 0.0), Eigen::Vector3f(0.0, 1.0, 0.0));
	MVPMatrix = p * v;
}

void RTTRenderer::setScreenSize(int _screenWidth, int _screenHeight) {
	screenWidth = _screenWidth;
	screenHeight = _screenHeight;
}

void RTTRenderer::setCameraIntrinsic(float _width, float _height, float _fx, float _fy, float _px, float _py) {
	width = _width;
	height = _height;

	Eigen::Matrix4f m;
	m.setIdentity();
	if (width < height) {
		m(0, 0) *= 2.5;
		m(1, 1) *= 2.5 * height / width;
	}
	else {
		m(0, 0) *= 2.5 * width / height;
		m(1, 1) *= 2.5;
	}

	m(0, 3) = -3 + 0.5 * m(0, 0);
	m(1, 3) = 3 - 0.5 * m(1, 1);

	MVPMatrix = MVPMatrix * m;

	rtt.setCameraIntrinsic(width, height, _fx, _fy, _px, _py);
}

void RTTRenderer::onDraw()
{
	rtt.onDraw();

	shader.bind();
	glViewport(0, 0, screenWidth, screenHeight);

	glUniformMatrix4fv(MVPMatrixHandle, 1, GL_FALSE, MVPMatrix.data());

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(vertexHandle, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(vertexHandle);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(uvHandle, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(uvHandle);

	glUniform1i(textureHandle, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rtt.renderedTextureBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	shader.unbind();

}

void RTTRenderer::setCameraImage(cv::Mat& inputImage) {
	rtt.setCameraImage(inputImage);
}

void RTTRenderer::setMarkerPose(std::vector<std::pair<int, Eigen::Matrix4f>>& cameraViewPointPose) {
	rtt.setMarkerPose(cameraViewPointPose);
}