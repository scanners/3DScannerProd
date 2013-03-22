#include "stdafx.h"

#include "intrinsicController.h"
#include "calibrationModel.h"
#include "takePicView.h"


IntrinsicController::IntrinsicController() {
	
}

void IntrinsicController::createTakePicView() {
	takePicView = new TakePicView(Enums::controllerEnum::INTRINSIC, calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::INTRINSIC));
	takePicView->setCalibrationController(*this);
	takePicView->setModal(true);
	takePicView->show();
}

void IntrinsicController::findCorners(Mat image) {
	int successes = calibrationModel->findCorners(image);
	if (successes <= 0) {
		takePicView->showMessage(Enums::calibrationEnum::CORNERS_FAILURE);
	} else if (successes < calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::INTRINSIC)) {
		takePicView->incrementSuccesses(successes, calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::INTRINSIC));
		takePicView->showMessage(Enums::calibrationEnum::CORNERS_SUCCESS);
	} else if (successes == calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::INTRINSIC)) {
		calibrationModel->setImageForCornerDisplay(image);
		calibrationModel->calibrateIntrinsics();
		takePicView->incrementSuccesses(successes, calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::INTRINSIC));
		takePicView->showMessage(Enums::calibrationEnum::CALIBRATION_SUCCESS);
	}
}