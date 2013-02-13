#ifndef SCANMODEL_H
#define SCANMODEL_H

#include <vector>
using std::vector;
#include <string>
using std::string;
#include "plane.h"
#include "objectPoint.h"
#include "image.h"
using cv::Mat;

class ScanModel {
private:
	Mat intrinsics;
	vector<float> groundRegion;
	Mat groundExtrinsicTrans;
	Mat groundExtrinsicRot;
	Mat distortion;
	vector<float> backRegion;
	Mat backExtrinsicTrans;
	Mat backExtrinsicRot;
	string intrinsicFileName;
	string distortionFileName;
	string extBackRotFileName;
	string extBackTransFileName;
	string extGroundRotFileName;
	string extGroundTransFileName;
public:
	ScanModel();
	void scan();
	Plane* getLaserPlane();
	void convertCoords();
	int sendRegion(float);
	int getNumStoredCoords();
	vector<float>* sortStoredCoords();
	bool savePicture(Image);
	bool isDoneScanning();
	int buildImageObjects();
	void processImage(int);
	vector<ObjectPoint>* getObjectPoints();
	void saveFile(string);
	void exit();
	void loadDirectory(string);
};

#endif //SCANMODEL_H