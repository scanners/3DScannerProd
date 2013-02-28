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

void CalibrationController::saveFiles(string directory) {

}

void CalibrationController::setNumCorners(int horizontal, int vertical) {
	calibrationModel->setNumCorners(horizontal, vertical);
}