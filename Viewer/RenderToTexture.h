#pragma once

#include <vector>
#include <string>
#include <mutex>

#include <Eigen/Eigen>

#include "GL/glew.h"
#include "GL/freeglut.h"

#include "shader/Shader.h"

#include "QuadRenderer.h"
#include "AxisRenderer.h"

class RenderToTexture
{
public:
	RenderToTexture();
	~RenderToTexture();

	void initialize(const std::string& shader_folder_path);

	void setCameraIntrinsic(float width, float height, float fx, float fy, float px, float py);

	void onDraw();

	void setCameraImage(cv::Mat& inputImage);
	void setMarkerPose(std::vector<std::pair<int, Eigen::Matrix4f>>& inputPose);

	GLuint renderedTextureBuffer;
	GLuint depthRenderBuffer;

protected:
	float width;
	float height;
	float fx;
	float fy;
	float px;
	float py;

	QuadRenderer cameraRenderer;
	AxisRenderer axisRenderer;

	Shader shader;

	GLuint frameBufferHandle;

	GLuint renderedTextureHandle;
	GLenum attachment;

	Eigen::Matrix4f cameraMVPMatrix;
	Eigen::Matrix4f markerVPMatrix;

	cv::Mat image;
	std::vector<std::pair<int, Eigen::Matrix4f>> markerPoses;
	std::vector<std::pair<int, Eigen::Matrix4f>> markerMVP;

	bool updateMarkerPose = false;
	bool updateImage = false;
};
