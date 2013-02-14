#include "stdafx.h"

#include "intrinsicController.h"

IntrinsicController::IntrinsicController() {

}

void IntrinsicController::findCorners(Mat image) {
	int successes = calibrationModel.findCorners(image);
	calibrationModel.getMaxNumSuccesses(CalibrationController::INTRINSIC);
}