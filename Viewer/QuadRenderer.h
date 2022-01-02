#pragma once

#include <vector>
#include <string>
#include <mutex>

#include <Eigen/Eigen>

#include <opencv2/opencv.hpp>

#include "GL/glew.h"
#include "GL/freeglut.h"

#include "shader/Shader.h"

class QuadRenderer
{
public:
	QuadRenderer();
	~QuadRenderer();

	void initialize(const std::string& shaderFolderPath);
	void onDraw(Eigen::Matrix4f vpMatrix);

	void setModelMatrix(Eigen::Matrix4f matrix);
	void setCameraImage(cv::Mat& inputImage);
	void setTexture(GLuint texture);

protected:
	Shader shader;

	GLuint MVPMatrixHandle;
	GLuint vertexHandle;
	GLuint vertexBuffer;
	GLuint uvHandle;
	GLuint uvBuffer;
	GLuint textureBuffer;
	GLuint textureHandle;

	Eigen::Matrix4f MVPMatrix;
	Eigen::Matrix4f modelMatrix;

	bool updateImage = false;
};
