#include "RenderToTexture.h"

#include "GLMatrix.hpp"

using namespace std;

RenderToTexture::RenderToTexture()
{
}

RenderToTexture::~RenderToTexture()
{
}

void RenderToTexture::initialize(const string& _shaderFolderPath)
{
	cameraRenderer.initialize(_shaderFolderPath);

	axisRenderer.initialize(_shaderFolderPath);

	Eigen::Matrix4f axisModelMatrix;
	axisModelMatrix.setIdentity();
	axisModelMatrix(0, 0) = 5;
	axisModelMatrix(1, 1) = 5;
	axisModelMatrix(2, 2) = 5;
	axisRenderer.setModelMatrix(axisModelMatrix);
	axisRenderer.setLineWidth(3.0f);
}

void RenderToTexture::setCameraIntrinsic(float _width, float _height, float _fx, float _fy, float _px, float _py) {
	width = _width;
	height = _height;
	fx = _fx;
	fy = _fy;
	px = _px;
	py = _py;

	glGenFramebuffers(1, &frameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

	glGenTextures(1, &renderedTextureBuffer);
	glBindTexture(GL_TEXTURE_2D, renderedTextureBuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glGenTextures(1, &depthRenderBuffer);
	glBindTexture(GL_TEXTURE_2D, depthRenderBuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, width, height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthRenderBuffer, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTextureBuffer, 0);

	attachment = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	float nearClipPlane = 0.001f;
	float farClipPlane = 1000.0f;

	Eigen::Matrix4f p;
	p.setIdentity();
	p(0, 0) = 2.f * 640.f;
	p(1, 1) = 2.f * 640.f;
	p(2, 2) = -1.f;
	p(3, 2) = -1.f;
	p(3, 3) = 0.f;
	p(2, 2) = -(farClipPlane + nearClipPlane) / (farClipPlane - nearClipPlane);
	p(2, 3) = -(2 * farClipPlane * nearClipPlane) / (farClipPlane - nearClipPlane);

	Eigen::Matrix4f v = GLMatrix::lookAt(Eigen::Vector3f(0.0, 0.0, 640.0f), Eigen::Vector3f(0.0, 0.0, 0.0), Eigen::Vector3f(0.0, 1.0, 0.0));

	cameraMVPMatrix = p * v;

	p = GLMatrix::cameraProjectionMatrix(width, height, fx, fy, px, py, nearClipPlane, farClipPlane);
	v = GLMatrix::convetCameraAxisToGLAxis();

	markerVPMatrix = p * v;
}

void RenderToTexture::onDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
	glDrawBuffers(1, &attachment);

	glViewport(0, 0, width, height);
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.bind();

	cameraRenderer.onDraw(cameraMVPMatrix);

	if (updateMarkerPose == true) {
		int markerNum = markerPoses.size();

		markerMVP.resize(markerNum);
		for (int i = 0; i < markerNum; i++) {
			markerMVP[i].first = markerPoses[i].first;
			markerMVP[i].second = markerVPMatrix * markerPoses[i].second;
		}

		updateMarkerPose = false;
	}

	for (int i = 0; i < markerPoses.size(); i++) {
		axisRenderer.onDraw(markerMVP[i].second);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderToTexture::setCameraImage(cv::Mat& inputImage) {
	cameraRenderer.setCameraImage(inputImage);
}

void RenderToTexture::setMarkerPose(std::vector<std::pair<int, Eigen::Matrix4f>>& inputPose) {
	markerPoses = inputPose;
	updateMarkerPose = true;
}