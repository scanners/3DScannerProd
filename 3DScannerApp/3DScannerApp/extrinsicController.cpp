#include "stdafx.h"
#include "extrinsicController.h"
#include "calibrationModel.h"
#include "takePicView.h"


ExtrinsicController::ExtrinsicController() {
	
}

void ExtrinsicController::createTakePicView() {
	takePicView = new TakePicView(Enums::controllerEnum::EXTRINSIC);
	takePicView->setCalibrationController(*this);
	takePicView->setModal(true);
	takePicView->show();
}

void ExtrinsicController::findCorners(Mat image) {
	int successes = calibrationModel->findCorners(image);
	if (successes <= 0) {
		takePicView->showMessage(Enums::calibrationEnum::CORNERS_FAILURE);
	} else if (successes < calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::EXTRINSIC)) {
		calibrationModel->calibrateExtrinsics(Enums::extrinsicBoardLocation::BACK_PLANE);
		takePicView->incrementSuccesses(successes, calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::EXTRINSIC));
		takePicView->showMessage(Enums::calibrationEnum::CORNERS_SUCCESS);
	} else if (successes == calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::EXTRINSIC)) {
		calibrationModel->calibrateExtrinsics(Enums::extrinsicBoardLocation::GROUND_PLANE);
		calibrationModel->saveExtrinsicFiles();
		takePicView->showMessage(Enums::calibrationEnum::CALIBRATION_SUCCESS);
	}
}