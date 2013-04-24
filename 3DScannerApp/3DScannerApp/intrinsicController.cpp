#include "stdafx.h"

#include "intrinsicController.h"
#include "calibrationModel.h"
#include "takePicView.h"
#include <qmessagebox.h>

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
		bool saveSuccess = calibrationModel->saveIntrinsicFiles();
		if (!saveSuccess) {
			QMessageBox message;
			message.setIcon(QMessageBox::Information);
			message.setWindowTitle("Error Saving File");
			message.setText("There was an error saving the file. Please choose a \n"
				"different directory or run the program as an Administrator.");
			message.exec();
		} else {
			takePicView->incrementSuccesses(successes, calibrationModel->getRequiredNumSuccesses(Enums::controllerEnum::INTRINSIC));
			takePicView->showMessage(Enums::calibrationEnum::CALIBRATION_SUCCESS);
		}
	}
}

IntrinsicController::~IntrinsicController()
{
	delete takePicView;
}