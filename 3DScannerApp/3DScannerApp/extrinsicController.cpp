#include "stdafx.h"

#include "extrinsicController.h"

ExtrinsicController::ExtrinsicController() {

}

bool ExtrinsicController::loadXML() {
	return false;
}

void ExtrinsicController::findCorners(Mat image) {
	int successes = calibrationModel.findCorners(image);
	calibrationModel.getMaxNumSuccesses(CalibrationController::EXTRINSIC);
}