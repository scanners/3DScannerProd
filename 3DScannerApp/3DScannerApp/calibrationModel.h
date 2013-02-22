#ifndef CALIBRATIONMODEL_H
#define CALIBRATIONMODEL_H

//////////////////  CHANGE THESE VALUES /////////////////////////////////////
#define INTRINSIC_MAX_NUM_SUCCESSES 10
#define EXTRINSIC_MAX_NUM_SUCCESSES 5
#define NUM_BOARDS_TO_FIND_CORNERS 20

#include <string>
using namespace::cv;
#include "enums.h"

class CalibrationController;

class CalibrationModel {
private:
	Size innerCorners;
	Size imageSize;
	int corners;
	vector<vector<Point3f>> objectPoints;
    vector<vector<Point2f>> imagePoints;
	int maxSuccesses;
	int successes;
	Mat intrinsicMatrix;
	Mat distortionCoefficients;
	vector<Mat> rotationVectors;
	vector<Mat> translationVectors;
	string intrinsicFileName;
	string extBackRotFileName;
	string extBackTransFileName;
	string extGroundRotFileName;
	string extGroundTransFileName;
	CalibrationController* calibrationController;
public:
	CalibrationModel(int horizontal, int vertical);
	void setCalibrationController(CalibrationController& controller);
	void saveFiles(string directory);
	bool loadXML(string directory);
	int getMaxNumSuccesses(int controllerType);
	int findCorners(Mat image);
	void calibrateIntrinsics();
	int startCalibration(int horizontal, int vertical);
};

#endif // CALIBRATIONMODEL_H