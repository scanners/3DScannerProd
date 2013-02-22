#ifndef CALIBRATIONCONTROLLER_H
#define CALIBRATIONCONTROLLER_H

#include "stdafx.h"
#include <string>
using std::string;
using cv::Mat;

class CalibrationModel;
class TakePicView;

class CalibrationController {
private:
	//SaveView saveView;
	//InputView inputView;
protected:
	CalibrationModel * calibrationModel;
	TakePicView * calibPicView;
public:
	void takePicture();
	void startCalibration(int, int);
	void setCalibrationModel(CalibrationModel& calibModel);
	void saveFiles(string);
	void findCorners(Mat image); 
};

#endif //CALIBRATIONCONTROLLER_H