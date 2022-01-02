/*==============================================================================
Copyright 2017 Maxst, Inc. All Rights Reserved.
==============================================================================*/
#pragma once

#include <vector>
#include <string>
#include <mutex>

#include <Eigen/Eigen>

#include "GL/glew.h"
#include "GL/freeglut.h"

#include "shader/Shader.h"

#include "RenderToTexture.h"

class RTTRenderer
{
public:
	RTTRenderer();
	~RTTRenderer();

	void initialize(const std::string& shader_folder_path);

	void setScreenSize(int screenWidth, int screenHeight);
	void setCameraIntrinsic(float width, float height, float fx, float fy, float px, float py);

	void onDraw();

	void setCameraImage(cv::Mat& inputImage);
	void setMarkerPose(std::vector<std::pair<int, Eigen::Matrix4f>>& inputPose);

protected:
	RenderToTexture rtt;

	float width;
	float height;

	int screenWidth;
	int screenHeight;

	Shader shader;

	GLuint MVPMatrixHandle;
	GLuint vertexHandle;
	GLuint vertexBuffer;
	GLuint uvHandle;
	GLuint uvBuffer;
	GLuint textureHandle;

	Eigen::Matrix4f MVPMatrix;
};
