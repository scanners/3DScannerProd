#ifndef INTRINSICCONTROLLER_H
#define INTRINSICCONTROLLER_H

#include "calibrationController.h"

class IntrinsicController : public CalibrationController {
public:
	IntrinsicController();
	void createTakePicView();
	void findCorners(Mat image);
};

#endif //INTRINSICCONTROLLER_H