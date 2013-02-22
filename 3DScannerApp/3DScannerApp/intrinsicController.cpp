#include "stdafx.h"

#include "intrinsicController.h"
#include "takePicView.h"
#include "enums.h"

IntrinsicController::IntrinsicController() {
	calibPicView = new TakePicView(Enums::controllerEnum::INTRINSIC);
	calibPicView->setModal(true);
	calibPicView->show();
	calibPicView->setCalibrationController(*this);
}