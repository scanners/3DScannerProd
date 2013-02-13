#ifndef CALIBRATIONCONTROLLER_H
#define CALIBRATIONCONTROLLER_H

#include <string>
using std::string;
#include "calibrationModel.h"

class CalibrationController {
private:
	//TakePicView: takePicView;
	//SaveView saveView;
	//InputView inputView;
	CalibrationModel calibrationModel;
public:
	void takePicture();
	void startCalibration();
	void setCalibrationModel(CalibrationModel);
	void saveFiles(string);
	int findCorners();
};

#endif //CALIBRATIONCONTROLLER_H