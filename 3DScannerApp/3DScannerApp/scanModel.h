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
	vector<Mat> redChannels;
	vector<vector<Point3f>> objectPoints;
	string saveDirectory;
	string loadDirectory;
	int imageWidth;
	int imageHeight;
	int numImages;
	int topOfBackPlane;
	int bottomOfBackPlane;
	int topOfGroundPlane;
	int bottomOfGroundPlane;
	bool scanComplete;
	vector<vector<Point2f>> redPointsInBackPlaneLine;
	vector<vector<Point2f>> redPointsInGroundPlaneLine;
	vector<vector<Point2f>> redPointsOnObject;
	Plane findLaserPlane(vector<Point2f> backPlanePoints, vector<Point2f> groundPlanePoints);
	vector<Point3f> findObjectLaserIntersections(Plane laserPlane, vector<Point2f> redPointsOnObject);
	vector<Point3f> findRayPlaneIntersections(int planeLocation, vector<Point2f> imagePoints);
	Vec6f findBestFitRedLine(vector<Point3f> redPointsInCameraCoords);
	Point3f findLineLineIntersection(Vec6f backLine, Vec6f groundLine);
	Point3f findLaserPlaneNormalVector(Vec6f backLine, Vec6f groundLine);
	float findMidpointRedComponentAtPixel(int x, int y, vector<Mat> redChannels);
	void findDifferenceImages();
	void findDifferenceImageAtPixel(int x, int y, vector<Mat>& redChannels);
	void findRedPoints();
	vector<Point2f> findRedPointsInRegion(int region, int imageNum);
	Point2f findZeroCrossingInRow(int y, int imageNum);
public:
	ScanModel();
	void scan();
	void processScan();
	void resetScan();
	void convertCoords();
	void setSaveDirectory(string saveDir);
	void setLoadDirectory(string loadDir);
	int setRegion(int yCoordinate);
	int getNumStoredCoords();
	void sortStoredYCoords();
	vector<Point> getRegionPixels();
	void storeRedChannel(Mat image);
	void setImageWidth(Mat image);
	int getImageWidth();
	bool savePicture(Image * image);
	bool loadXML();
	bool isDoneScanning();
	int buildImageObjects();
	//Take out pointer when implementing
	vector<ObjectPoint>* getObjectPoints();
	int getRequiredNumStoredYCoords();
	void saveFile(string fileName);
	void exit();
	void resetRegions();
	bool isDone(int done, int total);
};

#endif //SCANMODEL_H