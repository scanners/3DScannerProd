#include "stdafx.h"

#include "calibrationModel.h"
#include "calibrationController.h"
#include "takePicView.h"

void CalibrationController::setCalibrationModel(CalibrationModel& calibModel) {
	calibrationModel = &calibModel;
}

void CalibrationController::setInputView(InputView& inpView) {
	inputView = &inpView;
}

bool CalibrationController::loadXML() {
	return calibrationModel->loadXML();
}

void CalibrationController::setNumCorners(int horizontal, int vertical) {
	calibrationModel->setNumCorners(horizontal, vertical);
}

void CalibrationController::resetSuccesses() {
	calibrationModel->resetSuccesses();
}

void CalibrationController::setSaveDirectory(string directory) {
	calibrationModel->setSaveDirectory(directory);
}

void CalibrationController::setLoadDirectory(string directory) {
	calibrationModel->setLoadDirectory(directory);
}