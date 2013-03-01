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
protected:
	CalibrationModel * calibrationModel;
	InputView * inputView;
	TakePicView * takePicView;
public:
	void setCalibrationModel(CalibrationModel& calibModel);
	void saveFiles(string);
	virtual void findCorners(Mat image) = 0;
	bool loadXML();
	void setInputView(InputView& inpView);
	void setNumCorners(int horizontal, int vertical);
	void setSaveDirectory(string directory);
	void setLoadDirectory(string directory);
	virtual void createTakePicView() = 0;
};

#endif //CALIBRATIONCONTROLLER_H