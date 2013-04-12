#ifndef SCANMODEL_H
#define SCANMODEL_H

#define REQUIRED_NUM_STORED_Y_COORDS 4

#include <vector>
using std::vector;
#include <string>
#include <stdio.h>
#include "Serial.h"

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
	int processedImages;
	int processedRows;
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
	float findMidpointRedComponentInRow(int y);
	void findDifferenceImageAtPixel(int x, int y, float midpointRedComponent);
	vector<Point2f> findRedPointsInRegion(int region, int imageNum);
	Point2f findZeroCrossingInRow(int y, int imageNum);
public:
	ScanModel();
	~ScanModel();
	int ShowError (LONG lError, LPCTSTR lptszMessage);
	int scan();
	bool isDoneScanning(CSerial &serial, LONG &lLastError);
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
	void findNextDifferenceImage(int y);
	void findNextRedPoints(int imageNum);
	int buildImageObjects();
	//Take out pointer when implementing
	vector<ObjectPoint>* getObjectPoints();
	int getRequiredNumStoredYCoords();
	int getTopRowToProcess();
	int getBottomRowToProcess();
	void saveFile(string fileName);
	void exit();
	void resetRegions();
	bool isDoneFindingFindingDifferenceImages();
	bool isDoneFindingRedPoints();
	bool isDoneProcessingFrames();
	void processNextFrame(int imageNum);
	int getNumImages(); // returns how many images there are to process
	int getProcessedImages(); // returns the number of processed images
	void createPointCloud();
};

#endif //SCANMODEL_H