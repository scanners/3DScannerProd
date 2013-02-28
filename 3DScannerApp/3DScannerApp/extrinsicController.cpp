#include "stdafx.h"
#include "extrinsicController.h"
#include "takePicView.h"
#include "enums.h"

ExtrinsicController::ExtrinsicController() {
	
}

bool ExtrinsicController::loadXML() {
	return false;
}

void ExtrinsicController::createTakePicView() {
	takePicView = new TakePicView(Enums::controllerEnum::EXTRINSIC);
	takePicView->setCalibrationController(*this);
	takePicView->setModal(true);
	takePicView->show();
}