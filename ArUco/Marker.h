#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <Eigen/Eigen>

struct Marker
{
	int id = -1;
	double size = 1.0;
	Eigen::Matrix4d pose;
	cv::aruco::PREDEFINED_DICTIONARY_NAME pattern;
	cv::Mat image;
};