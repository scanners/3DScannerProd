#include "stdafx.h"
#include "extrinsicController.h"
#include "calibrationModel.h"
#include "takePicView.h"
#include <qmessagebox.h>

ExtrinsicController::ExtrinsicController() {
	
}

void ExtrinsicController::createTakePicView() {
	takePicView = new TakePicView(Enums::controllerEnum::EXTRINSIC, calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::EXTRINSIC));
	takePicView->setCalibrationController(*this);
	takePicView->setModal(true);
	takePicView->show();
}

void ExtrinsicController::findCorners(Mat image) {
	int successes = calibrationModel->findCorners(image);
	if (successes <= 0) {
		takePicView->showMessage(Enums::calibrationEnum::CORNERS_FAILURE);
	} else if (successes < calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::EXTRINSIC)) {
		calibrationModel->calibrateExtrinsics(Enums::boardLocation::BACK_PLANE);
		takePicView->incrementSuccesses(successes, calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::EXTRINSIC));
		takePicView->showMessage(Enums::calibrationEnum::CORNERS_SUCCESS);
	} else if (successes == calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::EXTRINSIC)) {
		calibrationModel->calibrateExtrinsics(Enums::boardLocation::GROUND_PLANE);
		bool saveSuccess = calibrationModel->saveExtrinsicFiles();
		if (!saveSuccess) {
			QMessageBox message;
			message.setIcon(QMessageBox::Information);
			message.setWindowTitle("Error Saving File");
			message.setText("There was an error saving the file. Try choosing a \n"
				"different directory or running the program as an Administrator.");
			message.exec();
		} else {
			takePicView->incrementSuccesses(successes, calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::EXTRINSIC));
			takePicView->showMessage(Enums::calibrationEnum::CALIBRATION_SUCCESS);
		}
	}
}

ExtrinsicController::~ExtrinsicController()
{
	delete takePicView;
}