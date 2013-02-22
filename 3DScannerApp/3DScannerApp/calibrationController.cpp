#include "stdafx.h"

#include "calibrationModel.h"
#include "calibrationController.h"

void CalibrationController::takePicture() {

}

void CalibrationController::startCalibration(int horizontal, int vertical) {
	//int enum = calibrationModel.startCalibration(horizontal, vertical);
}

void CalibrationController::setCalibrationModel(CalibrationModel& calibModel) {
	calibrationModel = &calibModel;
}

void CalibrationController::saveFiles(string directory) {

}

void CalibrationController::findCorners(Mat image) {
	int successes = calibrationModel->findCorners(image);
	//calibrationModel->getMaxNumSuccesses(Enums::controllerEnum::EXTRINSIC);
}