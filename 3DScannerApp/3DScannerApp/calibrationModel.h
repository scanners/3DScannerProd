#ifndef CALIBRATIONMODEL_H
#define CALIBRATIONMODEL_H

#include <string>
using std::string;
using cv::Mat;

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
public:
	CalibrationModel();
	void saveFiles(string);
	bool loadXML();
	int getMaxNumSuccesses();
	int findCorners();
	bool calibrateIntrinsics();
};

#endif // CALIBRATIONMODEL_H