#include "stdafx.h"

#include "intrinsicController.h"
#include "takePicView.h"
#include "enums.h"

IntrinsicController::IntrinsicController() {
	
}

void IntrinsicController::createTakePicView() {
	takePicView = new TakePicView(Enums::controllerEnum::INTRINSIC);
	takePicView->setCalibrationController(*this);
	takePicView->setModal(true);
	takePicView->show();
}