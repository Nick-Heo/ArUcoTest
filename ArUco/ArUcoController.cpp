#include "ArUcoController.h"

#include <opencv2/core/eigen.hpp>

ArUcoController::ArUcoController() {
	cvMarker3D.push_back(cv::Point3f(-0.5f, 0.5f, 0.0f));
	cvMarker3D.push_back(cv::Point3f(0.5f, 0.5f, 0.0f));
	cvMarker3D.push_back(cv::Point3f(0.5f, -0.5f, 0.0f));
	cvMarker3D.push_back(cv::Point3f(-0.5f, -0.5f, 0.0f));

	dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	params = cv::aruco::DetectorParameters::create();
}

ArUcoController::~ArUcoController() {
}

void ArUcoController::setCameraIntrinsic(double width, double height, double fx, double fy, double px, double py) {
	cvK = (cv::Mat_<double>(3, 3) <<
		fx, 0.0, px,
		0.0, fy, py,
		0.0, 0.0, 1.0
		);

	imageWidth = width;
	imageHeight = height;
}

void ArUcoController::setArUcoPattern(cv::aruco::PREDEFINED_DICTIONARY_NAME _pattern) {
	pattern = _pattern;
	dictionary = cv::aruco::getPredefinedDictionary(pattern);

	if (pattern < 4) {
		syntheticSize = 6;
	}
	else if(pattern < 8) {
		syntheticSize = 7;
	}
	else if (pattern < 12) {
		syntheticSize = 8;
	}
	else if (pattern < 16) {
		syntheticSize = 9;
	}
	else {
		syntheticSize = 128;
	}

	syntheticMarker.clear();
}

void ArUcoController::presetMarkerSize(int id, double size) {
	markerSize[id] = size;
}

void ArUcoController::clearPresetMarkerSize() {
	markerSize.clear();
}

int ArUcoController::calculateArUcoPose(cv::Mat& image, std::vector<Marker>& outMarkers) {
	if (imageWidth <= 0 || imageHeight <= 0) {
		setCameraIntrinsic(image.cols, image.rows, image.cols, image.cols, image.cols / 2.0, image.rows / 2.0);
	}

	std::vector<std::vector<cv::Point2f>> corners;
	std::vector<int> ids;

	cv::aruco::detectMarkers(image, dictionary, corners, ids, params);

	int detectedMarkerNum = (int)ids.size();
	outMarkers.clear();
	outMarkers.resize(ids.size());

	for (int i = 0; i < ids.size(); i++) {
		//for (int j = 0; j < corners[i].size(); j++) {
		//	cv::circle(debugImage, corners[i][j], syntheticSize, cv::Scalar(0, 255, 0), 2);
		//}

		if (syntheticMarker.find(ids[i]) == syntheticMarker.end()) {
			cv::Mat marker;
			cv::aruco::drawMarker(dictionary, ids[i], syntheticSize, marker);
			//cv::imshow("marker " + std::to_string(ids[i]), marker);
			syntheticMarker[ids[i]] = marker.clone();
		}

		double size = 1.0;
		cv::Mat rvec;
		cv::Mat tvec;
		if (markerSize.find(ids[i]) == markerSize.end()) {
			cv::solvePnP(cvMarker3D, corners[i], cvK, cv::noArray(), rvec, tvec, false, cv::SOLVEPNP_IPPE_SQUARE);
		}
		else {
			size = markerSize[ids[i]];
			std::vector<cv::Point3d> resizeCvMarker3D;
			resizeCvMarker3D.reserve(cvMarker3D.size());

			for (int j = 0; j < cvMarker3D.size(); j++) {
				resizeCvMarker3D.push_back(cvMarker3D[j] * size);
			}

			cv::solvePnP(resizeCvMarker3D, corners[i], cvK, cv::noArray(), rvec, tvec, false, cv::SOLVEPNP_IPPE_SQUARE);
		}


		cv::Mat rmat;
		cv::Rodrigues(rvec, rmat);

		Eigen::Matrix3d Rcw;
		cv::cv2eigen(rmat, Rcw);

		Eigen::Vector3d Tcw;
		cv::cv2eigen(tvec, Tcw);

		Eigen::Matrix4d Mcw;
		Mcw.setIdentity();
		Mcw.block<3, 3>(0, 0) = Rcw;
		Mcw.block<3, 1>(0, 3) = Tcw;

		outMarkers[i].id = ids[i];
		outMarkers[i].size = size;
		outMarkers[i].pose = Mcw;
		outMarkers[i].pattern = pattern;
		outMarkers[i].image = syntheticMarker[ids[i]].clone();
	}

	return detectedMarkerNum;
}