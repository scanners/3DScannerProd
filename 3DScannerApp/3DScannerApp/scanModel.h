#ifndef SCANMODEL_H
#define SCANMODEL_H

#define REQUIRED_NUM_STORED_Y_COORDS 4
#define REQUIRED_NUM_STORED_X_COORDS 2

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
	vector<int> regionXCoordinates;
	vector<Mat> redChannels;
	vector<vector<Point3d>> objectPoints;
	string saveDirectory;
	string saveFileName;
	string loadDirectory;
	int imageWidth;
	int numImages;
	int processedImages;
	int processedRows;
	int topOfBackPlane;
	int bottomOfBackPlane;
	int topOfGroundPlane;
	int bottomOfGroundPlane;
	int leftSideOfObject;
	int rightSideOfObject;
	bool scanComplete;
	vector<vector<Point2d>> redPointsInBackPlaneLine;
	vector<vector<Point2d>> redPointsInGroundPlaneLine;
	vector<vector<Point2d>> redPointsOnObject;
	Plane findLaserPlane(vector<Point2d> backPlanePoints, vector<Point2d> groundPlanePoints);
	vector<Point3d> findObjectLaserIntersections(Plane laserPlane, vector<Point2d> redPointsOnObject);
	vector<Point3f> findRayPlaneIntersections(int planeLocation, vector<Point2d> imagePoints);
	void convertPointsToHomogeneous(vector<Point2d> & imagePoints, vector<Point3d> & homogeneousCoords);
	Vec6d findBestFitRedLine(vector<Point3f> redPointsInCameraCoords);
	Point3d findLineLineIntersection(Vec6d backLine, Vec6d groundLine);
	Point3d findLaserPlaneNormalVector(Vec6d backLine, Vec6d groundLine);
	double vectorLength(Mat vector);
	double findMidpointRedComponentInRow(int y);
	void findDifferenceImageAtPixel(int x, int y, double midpointRedComponent);
	vector<Point2d> findRedPointsInRegion(int region, int imageNum);
	Point2d findZeroCrossingInRow(int y, int imageNum);
public:
	ScanModel();
	~ScanModel();
	int ShowError (LONG lError, LPCTSTR lptszMessage);
	int scan();
	bool isDoneScanning(CSerial &serial, LONG &lLastError);
	void resetScan();
	void setSaveDirectory(string saveDir);
	void setSaveFileName(string fileName);
	void setLoadDirectory(string loadDir);
	int setYRegion(int yCoordinate);
	int setXRegion(int xCoordinate);
	void sortStoredYCoords();
	void sortStoredXCoords();
	vector<Point> getRegionYPixels();
	vector<Point> getRegionXPixels();
	void storeRedChannel(Mat image);
	void setImageWidth(Mat image);
	int getImageWidth();
	bool loadXML();
	void findNextDifferenceImage(int y);
	void findNextRedPoints(int imageNum);
	int getRequiredNumStoredYCoords();
	int getRequiredNumStoredXCoords();
	int getTopRowToProcess();
	int getBottomRowToProcess();
	void resetRegions();
	bool isDoneFindingFindingDifferenceImages();
	bool isDoneFindingRedPoints();
	bool isDoneProcessingFrames();
	void processNextFrame(int imageNum);
	int getNumImages(); // returns how many images there are to process
	int getProcessedImages(); // returns the number of processed images
	bool createPointCloud();
};

#endif //SCANMODEL_H