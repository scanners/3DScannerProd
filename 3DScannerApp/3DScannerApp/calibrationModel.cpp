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
	return 0;
}

bool CalibrationModel::calibrateIntrinsics() {

	return false;
}

int CalibrationModel::startCalibration(int horizontal, int vertical) {
	int innerCorners = (horizontal - 1) * (vertical - 1);
	return Enums::calibrationEnum::TAKE_PIC;
}