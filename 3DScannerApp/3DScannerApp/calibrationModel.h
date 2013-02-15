#ifndef CALIBRATIONMODEL_H
#define CALIBRATIONMODEL_H

//////////////////  CHANGE THESE VALUES /////////////////////////////////////
#define INTRINSIC_MAX_NUM_SUCCESSES 10
#define EXTRINSIC_MAX_NUM_SUCCESSES 5

#include <string>
using namespace::cv;
#include "enums.h"

class CalibrationController;

class CalibrationModel {
private:
	int corners;
	vector<vector<Point3f>> objectPoints;
    vector<vector<Point2f>> imagePoints;
	int maxSuccesses;
	Mat intrinsic;
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
	int startCalibration(int, int);
};

#endif // CALIBRATIONMODEL_H