#ifndef CALIBRATIONCONTROLLER_H
#define CALIBRATIONCONTROLLER_H

#include "stdafx.h"
#include <string>
using std::string;
using cv::Mat;

class CalibrationModel;
class InputView;
class TakePicView;

class CalibrationController {
private:
	//SaveView saveView;
	//InputView inputView;
protected:
	CalibrationModel * calibrationModel;
	InputView * inputView;
	TakePicView * takePicView;
public:
	void takePicture();
	void setCalibrationModel(CalibrationModel& calibModel);
	void saveFiles(string);
	void findCorners(Mat image);
	void setInputView(InputView& inpView);
	void setNumCorners(int horizontal, int vertical);
	virtual void createTakePicView() = 0;
};

#endif //CALIBRATIONCONTROLLER_H