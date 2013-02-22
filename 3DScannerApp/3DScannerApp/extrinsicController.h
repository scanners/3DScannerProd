#ifndef EXTRINSICCONTROLLER_H
#define EXTRINSICCONTROLLER_H

#include "calibrationController.h"

class ExtrinsicController : public CalibrationController {
public:
	ExtrinsicController();
	bool loadXML();
	void findCorners(Mat);
};

#endif //EXTRINSICCONTROLLER_H