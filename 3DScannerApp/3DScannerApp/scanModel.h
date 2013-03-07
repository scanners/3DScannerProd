#ifndef SCANMODEL_H
#define SCANMODEL_H

#include <vector>
using std::vector;
#include <string>
using std::string;
using namespace::cv;

class Plane;
class ObjectPoint;
class Image;
class Intrinsic;
class Extrinsic;

class ScanModel {
private:
	Intrinsic * intrinsics;
	Extrinsic * backExtrinsics;
	Extrinsic * groundExtrinsics;
	vector<float> groundRegion;
	vector<float> backRegion;
	string saveDirectory;
	string loadDirectory;
public:
	ScanModel();
	void scan();
	Plane* getLaserPlane();
	void convertCoords();
	void setSaveDirectory(string saveDir);
	void setLoadDirectory(string loadDir);
	int sendRegion(float);
	int getNumStoredCoords();
	vector<float>* sortStoredCoords();
	bool savePicture(Image * image);
	bool loadXML();
	bool isDoneScanning();
	int buildImageObjects();
	void processImage(int);
	vector<ObjectPoint>* getObjectPoints();
	void saveFile(string);
	void exit();
};

#endif //SCANMODEL_H