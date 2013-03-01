#ifndef EXTRINSICCONTROLLER_H
#define EXTRINSICCONTROLLER_H

#include "calibrationController.h"

class ExtrinsicController : public CalibrationController {
public:
	ExtrinsicController();
	void createTakePicView();
	void findCorners(Mat image);
};

#endif //EXTRINSICCONTROLLER_H