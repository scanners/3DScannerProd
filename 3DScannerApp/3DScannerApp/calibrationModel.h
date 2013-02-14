#ifndef CALIBRATIONMODEL_H
#define CALIBRATIONMODEL_H

//////////////////  CHANGE THESE VALUES /////////////////////////////////////
#define INTRINSIC_MAX_NUM_SUCCESSES 10
#define EXTRINSIC_MAX_NUM_SUCCESSES 5

#include <string>
using std::string;
using cv::Mat;

class CalibrationController;

class CalibrationModel {
private:
	int verticalSquares;
	int maxSuccesses;
	Mat intrinsic;
	int horizontalSquares;
	Mat distortion;
	string intrinsicFileName;
	string extBackRotFileName;
	string extBackTransFileName;
	string extGroundRotFileName;
	string extGroundTransFileName;
	CalibrationController* calibrationController;
public:
	CalibrationModel();
	void setCalibrationController(CalibrationController*);
	void saveFiles(string);
	bool loadXML();
	int getMaxNumSuccesses(int);
	int findCorners(Mat);
	bool calibrateIntrinsics();
	string startCalibration(int, int);
};

#endif // CALIBRATIONMODEL_H