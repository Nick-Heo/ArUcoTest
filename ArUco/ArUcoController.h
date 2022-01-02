#pragma once

#include <vector>
#include <map>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include <Eigen/Eigen>

#include "Marker.h"

class ArUcoController
{
public:
	ArUcoController();
	~ArUcoController();

	void setCameraIntrinsic(double width, double height, double fx, double fy, double px, double py);
	void setArUcoPattern(cv::aruco::PREDEFINED_DICTIONARY_NAME pattern);
	void presetMarkerSize(int id, double size);
	void clearPresetMarkerSize();

	int calculateArUcoPose(cv::Mat& image, std::vector<Marker>& outMarkers);

private:
	cv::aruco::PREDEFINED_DICTIONARY_NAME pattern;
	cv::Ptr<cv::aruco::Dictionary> dictionary;
	cv::Ptr<cv::aruco::DetectorParameters> params;

	int syntheticSize = 100;
	std::map<int, cv::Mat> syntheticMarker;
	std::map<int, double> markerSize;

	cv::Mat cvK;
	std::vector<cv::Point3d> cvMarker3D;

	int imageWidth = -1;
	int imageHeight = -1;
};

