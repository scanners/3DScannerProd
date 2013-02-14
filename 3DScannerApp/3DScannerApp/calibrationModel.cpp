#include "stdafx.h"

#include "calibrationModel.h"
#include "calibrationController.h"

CalibrationModel::CalibrationModel() {

}

void CalibrationModel::setCalibrationController(CalibrationController* calibControl) {
	calibrationController = calibControl;
}

void CalibrationModel::saveFiles(string directory) {

}

bool CalibrationModel::loadXML() {
	return false;
}

int CalibrationModel::getMaxNumSuccesses(int controllerType) {
	if (controllerType == CalibrationController::INTRINSIC) {
		return INTRINSIC_MAX_NUM_SUCCESSES;
	} else if (controllerType == CalibrationController::EXTRINSIC) {
		return EXTRINSIC_MAX_NUM_SUCCESSES;
	} else {
		return -1;
	}

}

int CalibrationModel::findCorners(Mat image) {
	return 0;
}

bool CalibrationModel::calibrateIntrinsics() {
	return false;
}

string CalibrationModel::startCalibration(int horizontal, int vertical) {
	return "";
}