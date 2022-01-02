#include "Viewer.h"

#include "GLMatrix.hpp"

using namespace std;

Viewer::Viewer() {
}

Viewer::~Viewer() {
}

void Viewer::initialize(int argc, char **argv, const string &_shader_folder_path) {
	glutInit(&argc, argv);

	screenWidth = 1024;
	screenHeight = 1024;

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("ArUcoTest");
	glewInit();

	cameraCenter << 0.0f, 0.0f, -500.0f;
	lookCenter << 0.0f, 0.0f, 0.0f;
	upVector << 0.0f, -1.0f, 0.0f;
	orthoSize << 3.0f, 3.0f;
	orthoShift << 0.0f, 0.0f;
	viewMatrix.setIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	axisRenderer.initialize(_shader_folder_path);
	Eigen::Matrix4f axisModelMatrix;
	axisModelMatrix.setIdentity();
	axisModelMatrix(2, 3) = 0.01f;
	axisRenderer.setModelMatrix(axisModelMatrix);
	axisRenderer.setLineWidth(5.0f);
	cameraQuadRenderer.initialize(_shader_folder_path);
	cameraQuadRenderer.setScreenSize(screenWidth, screenHeight);

	markerRenderer.initialize(_shader_folder_path);
}

void Viewer::setCameraIntrinsic(float _width, float _height, float _fx, float _fy, float _px, float _py) {
	width = _width;
	height = _height;
	fx = _fx;
	fy = _fy;
	px = _px;
	py = _py;
	cameraQuadRenderer.setCameraIntrinsic(width, height, fx, fy, px, py);
}

void Viewer::onDraw() {

	glClearColor(0.1, 0.1, 0.1, 1.0);
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Eigen::Matrix4f p = GLMatrix::ortho(
		-orthoSize[0] + orthoShift.y(), orthoSize[0] + orthoShift.y(),
		-orthoSize[1] - orthoShift.x(), orthoSize[1] - orthoShift.x(),
		0.1f, 1000.f);

	Eigen::Matrix4f v = GLMatrix::lookAt(cameraCenter, lookCenter, upVector);

	MVPMatrix = p * v * viewMatrix;

	axisRenderer.onDraw(MVPMatrix);

	for (int i = 0; i < markers.size(); i++) {
		markerRenderer.setCameraImage(markers[i].image);
		Eigen::Matrix4f markerSizeMatrix;
		markerSizeMatrix.setIdentity();
		markerSizeMatrix(0, 0) = markers[i].size;
		markerSizeMatrix(1, 1) = markers[i].size;
		markerRenderer.setModelMatrix(markerSizeMatrix);
		markerRenderer.onDraw(p * v * viewMatrix * markers[i].pose.cast<float>());

		axisRenderer.onDraw(p * v * viewMatrix * markers[i].pose.cast<float>());
	}

	cameraQuadRenderer.onDraw();

	glutSwapBuffers();
	glutPostRedisplay();

	if (isExit == true) {
		glutLeaveMainLoop();
	}
}

void Viewer::onKeyPressed(unsigned char key, int x, int y) {
	switch (key) {
	case 'r':
		viewMatrix.setIdentity();
		break;
	case 's':
	{
		break;
	}
	case 27:
		isExit = true;
		break;
	default:
		break;
	}
}

void Viewer::onMouseClicked(int button, int state, int x, int y) {
	//button	left 0,
	//			right 2,
	//			middle 1.
	//			wheel up 3,
	//			wheel down 4
	//state click down 0, click up 1

	switch (button) {
	case 0:
		if (state == 0) {
			LClickedPos << x, y;
		}
		else if (state == 1) {
			Eigen::Vector2f diff(x - LClickedPos.x(), y - LClickedPos.y());

			orthoShift.x() += diff.x() * orthoSize[0] / 1000.f;
			orthoShift.y() += diff.y() * orthoSize[1] / 1000.f;
		}
		break;
	case 1:
		break;
	case 2:
		if (state == 0) {
			RClickedPos << x, y;
		}
		else if (state == 1) {
			Eigen::Vector2f diff(x - RClickedPos.x(), RClickedPos.y() - y);
			Eigen::Vector3f axis;
			if (diff.x() == 0) {
				axis << diff.y(), 0.0f, 0.0f;
			}
			else if (diff.y() == 0) {
				axis << 0.0f, diff.x(), 0.0f;
			}
			else {

				if (diff.x() > 0 && diff.y() > 0) {
					axis << 1.0f, -diff.x() / diff.y(), 0.f;
				}
				else if (diff.x() > 0 && diff.y() < 0) {
					axis << -1.0f, diff.x() / diff.y(), 0.f;
				}
				else if (diff.x() < 0 && diff.y() < 0) {
					axis << -1.0f, diff.x() / diff.y(), 0.f;
				}
				else if (diff.x() < 0 && diff.y() > 0) {
					axis << 1.0f, -diff.x() / diff.y(), 0.f;
				}
			}

			axis.normalize();

			float angle = -diff.norm() / 500.f;

			viewMatrix = GLMatrix::rotate(axis, angle) * viewMatrix;
		}

		break;
	case 3:
		orthoSize[0] = orthoSize[0] * 0.8f;
		orthoSize[1] = orthoSize[1] * 0.8f;

		if (orthoSize[0] < 0.1f) {
			orthoSize << 0.1f, 0.1f;
		}

		break;
	case 4:
		orthoSize[0] = orthoSize[0] * 1.3f;
		orthoSize[1] = orthoSize[1] * 1.3f;

		if (orthoSize[0] > 10000.f) {
			orthoSize << 10000.f, 10000.f;
		}
		break;
	}
}

void Viewer::setCameraImage(cv::Mat& inputImage) {
	cameraQuadRenderer.setCameraImage(inputImage);
}

void Viewer::setMarker(std::vector<Marker>& _markers) {
	markers = _markers;

	vector<pair<int, Eigen::Matrix4f>> Mcws;

	for (int i = 0; i < _markers.size(); i++) {
		Mcws.push_back(make_pair(_markers[i].id, _markers[i].pose.cast<float>()));
	}

	cameraQuadRenderer.setMarkerPose(Mcws);
}