#ifndef CALIBRATIONCONTROLLER_H
#define CALIBRATIONCONTROLLER_H

#include <string>
using std::string;
using cv::Mat;
#include "calibrationModel.h"

//class CalibrationModel;

class CalibrationController {
private:
	//TakePicView: takePicView;
	//SaveView saveView;
	//InputView inputView;
protected:
	CalibrationModel calibrationModel;
public:
	static enum calibrationTypes {INTRINSIC, EXTRINSIC};
	void takePicture();
	void startCalibration(int, int);
	void setCalibrationModel(CalibrationModel);
	void saveFiles(string);
	virtual void findCorners(Mat) = 0; 
};

#endif //CALIBRATIONCONTROLLER_H