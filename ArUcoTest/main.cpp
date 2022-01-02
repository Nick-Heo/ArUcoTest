#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <Eigen/Eigen>
#include <opencv2/core/eigen.hpp>

#include "Viewer.h"

#include "ArUcoController.h"

Viewer viewer;
ArUcoController arucoController;

cv::VideoCapture cap;

void onDraw() {

	cv::Mat image;
	cap >> image;

	std::vector<Marker> markers;
	int result = arucoController.calculateArUcoPose(image, markers);

	viewer.setMarker(markers);
	viewer.setCameraImage(image);
	
	viewer.onDraw();
}

void onKeyPressed(unsigned char key, int x, int y) {
	viewer.onKeyPressed(key, x, y);
}

void onMouseClicked(int button, int state, int x, int y) {
	viewer.onMouseClicked(button, state, x, y);
}

int main(int argc, char** argv) {
	viewer.initialize(argc, argv, "../Viewer/shader");

	cap.open(0);

	if (cap.isOpened() == false) {
		std::cout << "camera open fail " << std::endl;

		return -1;
	}

	cv::Mat testImage;
	cap >> testImage;

	double imageWidth = testImage.cols;
	double imageHeight = testImage.rows;

	arucoController.setArUcoPattern(cv::aruco::DICT_6X6_250);
	arucoController.setCameraIntrinsic(imageWidth, imageHeight, imageWidth, imageWidth, imageWidth / 2.0, imageHeight / 2.0);
	arucoController.presetMarkerSize(25, 1.16);
	arucoController.presetMarkerSize(33, 1.42);

	viewer.setCameraIntrinsic(imageWidth, imageHeight, imageWidth, imageWidth, imageWidth / 2.0, imageHeight / 2.0);

	glutDisplayFunc(onDraw);
	glutMouseFunc(onMouseClicked);
	glutKeyboardFunc(onKeyPressed);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glutMainLoop();

	return 0;
}
