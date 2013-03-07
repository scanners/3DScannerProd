#ifndef SCANMODEL_H
#define SCANMODEL_H

#define REQUIRED_NUM_STORED_Y_COORDS 4

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
	vector<int> regionYCoordinates;
	vector<int> groundRegion;
	vector<int> backRegion;
	string saveDirectory;
	string loadDirectory;
public:
	ScanModel();
	void scan();
	Plane* getLaserPlane();
	void convertCoords();
	void setSaveDirectory(string saveDir);
	void setLoadDirectory(string loadDir);
	int setRegion(int yCoordinate);
	int getNumStoredCoords();
	vector<int> sortedStoredYCoords();
	bool savePicture(Image * image);
	bool loadXML();
	bool isDoneScanning();
	int buildImageObjects();
	void processImage(int);
	vector<ObjectPoint>* getObjectPoints();
	int getRequiredNumStoredYCoords();
	void saveFile(string);
	void exit();
};

#endif //SCANMODEL_H