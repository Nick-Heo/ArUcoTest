#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <fstream>

#include <Eigen/Eigen>

#include "GL/glew.h"
#include "GL/freeglut.h"

#include "shader/Shader.h"

#include "AxisRenderer.h"
#include "RTTRenderer.h"

#include "Marker.h"

class Viewer
{
public:
	Viewer();
	~Viewer();

	void initialize(int argc, char **argv, const std::string& shaderFolderPath);

	void setCameraIntrinsic(float width, float height, float fx, float fy, float px, float py);

	//void onSizeChanged(int width, int height);
	void onDraw();
	void onKeyPressed(unsigned char key, int x, int y);
	void onMouseClicked(int button, int state, int x, int y);

	void setCameraImage(cv::Mat& inputImage);
	void setMarker(std::vector<Marker>& markers);

protected:
	AxisRenderer axisRenderer;
	QuadRenderer markerRenderer;
	RTTRenderer cameraQuadRenderer;

	Eigen::Matrix4f MVPMatrix;
	Eigen::Matrix4f viewMatrix;

	Eigen::Vector2f orthoSize;
	Eigen::Vector2f orthoShift;
	Eigen::Vector2f LClickedPos;
	Eigen::Vector2f RClickedPos;

	Eigen::Vector3f cameraCenter;
	Eigen::Vector3f lookCenter;
	Eigen::Vector3f upVector;

	std::vector<Marker> markers;

	int screenWidth;
	int screenHeight;

	float width;
	float height;
	float fx;
	float fy;
	float px;
	float py;

	bool isExit = false;
};
