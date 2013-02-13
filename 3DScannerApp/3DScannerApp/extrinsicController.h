#ifndef EXTRINSICCONTROLLER_H
#define EXTRINSICCONTROLLER_H

#include "calibrationController.h"

class ExtrinsicController : CalibrationController {
public:
	ExtrinsicController();
	bool loadXML();
};

#endif //EXTRINSICCONTROLLER_H