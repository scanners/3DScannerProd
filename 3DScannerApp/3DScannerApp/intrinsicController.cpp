#include "stdafx.h"

#include "intrinsicController.h"
#include "calibrationModel.h"
#include "takePicView.h"


IntrinsicController::IntrinsicController() {
	
}

void IntrinsicController::createTakePicView() {
	takePicView = new TakePicView(Enums::controllerEnum::INTRINSIC);
	takePicView->setCalibrationController(*this);
	takePicView->setModal(true);
	takePicView->show();
}

void IntrinsicController::findCorners(Mat image) {
	int successes = calibrationModel->findCorners(image);
	calibrationModel->getMaxNumSuccesses(Enums::controllerEnum::INTRINSIC);
	if (successes <= 0) {
		takePicView->showMessage(Enums::calibrationEnum::CORNERS_FAILURE);
	} else if (successes < calibrationModel->getMaxNumSuccesses(Enums::controllerEnum::INTRINSIC)) {
		takePicView->incrementSuccesses(successes, calibrationModel->getMaxNumSuccesses(Enums::controllerEnum::INTRINSIC));
		takePicView->showMessage(Enums::calibrationEnum::CORNERS_SUCCESS);
	} else if (successes == calibrationModel->getMaxNumSuccesses(Enums::controllerEnum::INTRINSIC)) {
		calibrationModel->calibrateIntrinsics();
		takePicView->showMessage(Enums::calibrationEnum::CALIBRATION_SUCCESS);
	}
}