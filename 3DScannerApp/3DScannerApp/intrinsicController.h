#ifndef INTRINSICCONTROLLER_H
#define INTRINSICCONTROLLER_H

#include "calibrationController.h"

class IntrinsicController : CalibrationController {
public:
	IntrinsicController();
	void findCorners(Mat);
};

#endif //INTRINSICCONTROLLER_H