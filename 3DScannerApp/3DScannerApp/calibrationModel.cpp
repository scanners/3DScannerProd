#include "stdafx.h"

#include "calibrationModel.h"
#include "calibrationController.h"

CalibrationModel::CalibrationModel() {

}

void CalibrationModel::setCalibrationController(CalibrationController* calibControl) {
	calibrationController = calibControl;
}

void CalibrationModel::saveFiles(string directory) {
	//CHECK FOR CONTROLLER TYPE AND ONLY SAVE RELEVANT FILE, i.e. intrinsics or extrinsics
	FileStorage fs(directory + "intrinsics.xml", FileStorage::WRITE);
	fs << "intrinsicMatrix" << intrinsicMatrix << "distortionCoefficients" << distortionCoefficients;
	fs.release();
}

bool CalibrationModel::loadXML(string directory) {
	FileStorage fs(directory + "intrinsics.xml", FileStorage::READ);
	fs["intrinsicMatrix"] >> intrinsicMatrix;
	fs["distortionCoefficients"] >> distortionCoefficients;
	fs.release();

	return false;
}

int CalibrationModel::getMaxNumSuccesses(int controllerType) {
	switch (controllerType) {
	case Enums::controllerEnum::INTRINSIC:
		return INTRINSIC_MAX_NUM_SUCCESSES;
	case Enums::controllerEnum::EXTRINSIC:
		return EXTRINSIC_MAX_NUM_SUCCESSES;
	default:
		return -1;
	}
}

int CalibrationModel::findCorners(Mat image) {
	Mat grayImage;
	bool foundCorrectNumCorners;
	//clock_t prevTimestamp = 0;
	//const Scalar RED(0,0,255), GREEN(0,255,0);
	//const char ESC_KEY = 27;
	vector<Point2f> pointBuffer;

	imageSize = image.size();
	foundCorrectNumCorners = findChessboardCorners(image, innerCorners, pointBuffer, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
	if (foundCorrectNumCorners) {
		successes++;
		cvtColor(image, grayImage, CV_BGR2GRAY);
		cornerSubPix(grayImage, pointBuffer, Size(11,11), Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
		imagePoints.push_back(pointBuffer);
		return successes;
	} else {
		return -1;
	}

}

void CalibrationModel::calibrateIntrinsics() {
	intrinsicMatrix = Mat::eye(3, 3, CV_64F);
	distortionCoefficients = Mat::zeros(8, 1, CV_64F);

	objectPoints.resize(imagePoints.size());
	calibrateCamera(objectPoints, imagePoints, imageSize, intrinsicMatrix,
		distortionCoefficients, rotationVectors, translationVectors);
}

int CalibrationModel::startCalibration(int horizontal, int vertical) {
	innerCorners = Size((horizontal - 1), (vertical - 1));

	return Enums::calibrationEnum::TAKE_PIC;
}