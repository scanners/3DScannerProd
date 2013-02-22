#include "stdafx.h"
#include "extrinsicController.h"
#include "takePicView.h"
#include "enums.h"

ExtrinsicController::ExtrinsicController() {
	calibPicView = new TakePicView(Enums::controllerEnum::EXTRINSIC);
	calibPicView->setModal(true);
	calibPicView->show();
	calibPicView->setCalibrationController(*this);
}

bool ExtrinsicController::loadXML() {
	return false;
}

