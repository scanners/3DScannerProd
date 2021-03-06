#include "stdafx.h"
#include <exception>
#include "scanModel.h"
#include "intrinsic.h"
#include "extrinsic.h"
#include "serialCommunication.h"
#include "image.h"
#include "plane.h"
#include "Serial.h"
#include <Windows.h>
#include <qmessagebox.h>
#include <fstream>
using std::ofstream;

ScanModel::ScanModel() : numImages(0), processedImages(0), processedRows(0), numTimesWritten(0), intrinsics(0), backExtrinsics(0),
groundExtrinsics(0), serial(), scanComplete(false), hardwareThread(0){
}

int ScanModel::getNumImages()
{
	return numImages;
}

int ScanModel::getProcessedImages()
{
	return processedImages;
}

void ScanModel::startHardwareScan(){
	serial->startStepperMotor();
}

void ScanModel::waitForHardwareScanComplete()
{
	serial = new SerialCommunication();
	
	hardwareThread = new QThread();

	QObject::connect(hardwareThread, SIGNAL(started()), serial, SLOT(receiveStopSignalFromHardware()));
	QObject::connect(serial, SIGNAL(sendErrorMessage(LONG, char *)), serial, SLOT(showError(LONG, char *)));
	serial->moveToThread(hardwareThread);

	hardwareThread->start();
}

bool ScanModel::isHardwareDoneScanning(){
	bool scanComplete = serial->getIsScanComplete();

	if(scanComplete && serial){
		hardwareThread->exit(0);
		//delete serial;
	}

	return scanComplete;
}

void ScanModel::storeRedChannel(Mat image) {
	vector<Mat> channels(image.channels());
	//Split the image into its 3 channels: B, G, R
	split(image, channels);
	//Convert red component uchar matrix to float matrix
	channels[2].convertTo(channels[2], CV_64F);
	redChannels.push_back(channels[2]);
	numImages++;
}

void ScanModel::findNextDifferenceImage(int y) {
	double midpointRedComponent;

	midpointRedComponent = this->findMidpointRedComponentInRow(y);
	//Only need difference image for the object region
	for (int x = leftSideOfObject; x < rightSideOfObject; x++) {
		this->findDifferenceImageAtPixel(x, y, midpointRedComponent);
	}
	
	processedRows++;
}

double ScanModel::findMidpointRedComponentInRow(int y) {
	//Set min to the maximum value so it gets set to a lower value
	double minRedComponent = 255;
	//Set max to the minimum value so it gets set to a higher value
	double maxRedComponent = 0;
	double midpointRedComponent;

	//Find midpoint across an entire row, rather than just the object region
	for (int x = 0; x < imageWidth; x++) {
		for (int n = 0; n < numImages; n++) {
			if (redChannels.at(n).at<double>(Point(x, y)) < minRedComponent) {
				minRedComponent = redChannels.at(n).at<double>(Point(x, y));
			}
			if (redChannels.at(n).at<double>(Point(x, y)) > maxRedComponent) {
				maxRedComponent = redChannels.at(n).at<double>(Point(x, y));
			}
		}
	}

	midpointRedComponent = (minRedComponent + maxRedComponent) / 2.0;
	return midpointRedComponent;
}

void ScanModel::findDifferenceImageAtPixel(int x, int y, double midpointRedComponent) {
	//Compute difference image. 
	for (int n = 0; n < numImages; n++) {
		redChannels.at(n).at<double>(Point(x, y)) = redChannels.at(n).at<double>(Point(x, y)) - midpointRedComponent;
	}
}

/**
This method will find the subpixel location of the red line at every row of every image.
The subpixels in the back plane region will be used to calculate the best fit line of the laser
on the back plane, and similarly for the ground plane.  There will be a pair of best fit lines
for each image (an integer).  These will be used to find the laser plane for each image (an integer).

The subpixels in the object region at a particular image define where the plane calculated for that
particular image intersects the object (e.g. if a laser plane was calculated for image #6, 
the subpixel locations of the red line on the object at image #6 define where plane #6 intersected the object).
From this information, we know where and when the laser plane intersects the object.
*/
void ScanModel::findNextRedPoints(int imageNum) {
	redPointsInBackPlaneLine.push_back(this->findRedPointsInRegion(Enums::scanRegion::BACK, imageNum));
	redPointsInGroundPlaneLine.push_back(this->findRedPointsInRegion(Enums::scanRegion::GROUND, imageNum));
	redPointsOnObject.push_back(this->findRedPointsInRegion(Enums::scanRegion::OBJECT, imageNum));
	processedImages++;
}

vector<Point2d> ScanModel::findRedPointsInRegion(int region, int imageNum) {
	vector<Point2d> redPoints;
	
	int startIndex;
	int upperBound;

	switch(region) {
	case Enums::scanRegion::BACK:
		startIndex = topOfBackPlane;
		upperBound = bottomOfBackPlane;
		break;
	case Enums::scanRegion::GROUND:
		startIndex = topOfGroundPlane;
		upperBound = bottomOfGroundPlane;
		break;
	case Enums::scanRegion::OBJECT:
		startIndex = bottomOfBackPlane;
		upperBound = topOfGroundPlane;
		break;
	}

	Point2d zeroCrossing;
	//Only need zero crossings between the top of the back plane and the bottom of the ground plane
	for (int y = startIndex; y < upperBound; y++) {
		//Find zero crossing from image left to image right
		zeroCrossing = this->findZeroCrossingInRow(y, imageNum);
		//If found
		if (zeroCrossing.x >= 0) {
			redPoints.push_back(zeroCrossing);
		}
	}

	return redPoints;
}

Point2d ScanModel::findZeroCrossingInRow(int y, int imageNum) {
	double interpolatedX;

	//Only need to find zero crossing in object region cols
	for (int x = leftSideOfObject; x < rightSideOfObject - 1; x++) {
		if ((redChannels.at(imageNum).at<double>(Point(x, y)) < 0.0) && (redChannels.at(imageNum).at<double>(Point(x + 1, y)) > 0.0)) {
			//Interpolate between x and x + 1 to find the x-value that crosses deltaRed = 0
			//Formula is: x_0 = x + (0 - deltaRed(x))/(deltaRed(x+1) - deltaRed(x))
			interpolatedX = (double)x + (0 - redChannels.at(imageNum).at<double>(Point(x, y))) / 
				(redChannels.at(imageNum).at<double>(Point(x + 1, y)) - redChannels.at(imageNum).at<double>(Point(x, y)));
			return Point2d(interpolatedX, (double)y);
		}
	}
	
	//Zero crossing not found
	return Point2d(-1.0, -1.0);
}

// imageNum is the index of the image we are processing
void ScanModel::processNextFrame(int imageNum)
{
	vector<Point3d> objectLaserIntersections;
	if ((redPointsInBackPlaneLine.at(imageNum).size() > 0) && (redPointsInGroundPlaneLine.at(imageNum).size() > 0)) {
		Plane laserPlane(this->findLaserPlane(redPointsInBackPlaneLine.at(imageNum), redPointsInGroundPlaneLine.at(imageNum)));
		objectLaserIntersections = this->findObjectLaserIntersections(laserPlane, redPointsOnObject.at(imageNum));
		if (objectLaserIntersections.size() > 0) {
			objectPoints.push_back(objectLaserIntersections);
		}
	}
	processedImages++;
}

Plane ScanModel::findLaserPlane(vector<Point2d> backPlanePoints, vector<Point2d> groundPlanePoints) {
	//Find red points in camera coords
	vector<Point3f> backPointsInCameraCoords = this->findRayPlaneIntersections(Enums::boardLocation::BACK_PLANE, backPlanePoints);
	vector<Point3f> groundPointsInCameraCoords = this->findRayPlaneIntersections(Enums::boardLocation::GROUND_PLANE, groundPlanePoints);

	//Find best fit line (in camera coordinates) of laser on back plane.
	//The best fit line is a 6f vector consisting of (vx, vy, vy), which is a vector of length one collinear to the line,
	//and (x0, y0, z0), which is a point on the line
	Vec6d backBestFitLine = this->findBestFitRedLine(backPointsInCameraCoords);
	Vec6d groundBestFitLine = this->findBestFitRedLine(groundPointsInCameraCoords);

	//Find approximate intersection and normal of the laser plane to build laser plane object
	Point3d approximateIntersection = this->findLineLineIntersection(backBestFitLine, groundBestFitLine);
	Vec3d normalVectorOfLaserPlane = this->findLaserPlaneNormalVector(backBestFitLine, groundBestFitLine);

	return Plane(approximateIntersection, normalVectorOfLaserPlane);
}

vector<Point3f> ScanModel::findRayPlaneIntersections(int boardLocation, vector<Point2d> imagePoints) {
	Mat planeOriginInWorldCoords(Point3d(0, 0, 0));
	Mat planeNormalVectorInWorldCoords(Vec3d(0, 0, 1));
	Mat cameraOriginInWorldCoords;
	
	static bool originConvertedToBack = false;
	static bool originConvertedToGround = false;

	if (boardLocation == Enums::boardLocation::BACK_PLANE) {
		if (!originConvertedToBack) {
			this->findCameraOriginInBackWorldCoords();
			originConvertedToBack = true;
		}
		cameraOriginInWorldCoords = this->cameraOriginInBackWorldCoords;
	} else if (boardLocation == Enums::boardLocation::GROUND_PLANE) {
		if (!originConvertedToGround) {
			this->findCameraOriginInGroundWorldCoords();
			originConvertedToGround = true;
		}
		cameraOriginInWorldCoords = this->cameraOriginInGroundWorldCoords;
	}

	vector<Point2d> undistortedImagePoints;
	vector<Point3d> pointsInCameraCoordsDouble;
	vector<Point3f> pointsInCameraCoordsFloat;

	undistortPoints(imagePoints, undistortedImagePoints, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	this->convertPointsToHomogeneous(undistortedImagePoints, pointsInCameraCoordsDouble);

	double lambda;
	Mat pointInCameraCoords;
	Mat convertedImagePoint;
	Mat pointOnPlaneInWorldCoords;

	//Calculate the lambda value of each red point on the plane and store the Camera coordinate of the red point on the back plane
	for (int i = 0; i < pointsInCameraCoordsDouble.size(); i++) {
		Mat(pointsInCameraCoordsDouble.at(i)).convertTo(pointInCameraCoords, CV_64F);
		if (boardLocation == Enums::boardLocation::BACK_PLANE) {
			convertedImagePoint = Mat(backExtrinsics->getRotationMatrix().inv() * pointInCameraCoords);
		} else if (boardLocation == Enums::boardLocation::GROUND_PLANE) {
			convertedImagePoint = Mat(groundExtrinsics->getRotationMatrix().inv() * pointInCameraCoords);
		}

		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(planeNormalVectorInWorldCoords.t() * (planeOriginInWorldCoords - cameraOriginInWorldCoords) / 
			(planeNormalVectorInWorldCoords.t() * convertedImagePoint)).at<double>(0, 0);
		pointInCameraCoords = lambda * pointInCameraCoords;

		pointsInCameraCoordsFloat.push_back(Point3d((float)pointInCameraCoords.at<double>(0,0), 
			(float)pointInCameraCoords.at<double>(1,0), (float)pointInCameraCoords.at<double>(2,0)));
	}

	return pointsInCameraCoordsFloat;
}

void ScanModel::findCameraOriginInBackWorldCoords() {
	Mat cameraOriginInCameraCoords(Point3d(0, 0, 0));
	cameraOriginInBackWorldCoords = Mat(backExtrinsics->getRotationMatrix().inv() * (cameraOriginInCameraCoords -
		backExtrinsics->getTranslationMatrix()));
}

void ScanModel::findCameraOriginInGroundWorldCoords() {
	Mat cameraOriginInCameraCoords(Point3d(0, 0, 0));
	cameraOriginInGroundWorldCoords = Mat(groundExtrinsics->getRotationMatrix().inv() * (cameraOriginInCameraCoords -
		groundExtrinsics->getTranslationMatrix()));
}

void ScanModel::convertPointsToHomogeneous(vector<Point2d> & imagePoints, vector<Point3d> & homogeneousCoords) {
	for (int i = 0; i < imagePoints.size(); i++) {
		homogeneousCoords.push_back(Point3d(imagePoints.at(i).x, imagePoints.at(i).y, 1));
	}
}

Vec6d ScanModel::findBestFitRedLine(vector<Point3f> redPointsInCameraCoords) {
	Vec6f bestFitLineFloat;
	fitLine(redPointsInCameraCoords, bestFitLineFloat, CV_DIST_L2, 0, 0.01, 0.01);

	Vec6d bestFitLineDouble;
	for (int i = 0; i < bestFitLineFloat.rows; i++) {
		bestFitLineDouble(i) = bestFitLineFloat(i);
	}
	return bestFitLineDouble;
}

Point3d ScanModel::findLineLineIntersection(Vec6d backLine, Vec6d groundLine) {
	Vec3d normalizedVectorOfBackLine(backLine[0], backLine[1], backLine[2]);
	Vec3d pointOnBackLine(backLine[3], backLine[4], backLine[5]);

	Vec3d normalizedVectorOfGroundLine(groundLine[0], groundLine[1], groundLine[2]);
	Vec3d pointOnGroundLine(groundLine[3], groundLine[4], groundLine[5]);

	//Compute ||v||^2 = v_x^2 + v_y^2 + v_z^2
	double squaredMagnitudeOfBackLineVector = std::pow(normalizedVectorOfBackLine[0], 2) +
		std::pow(normalizedVectorOfBackLine[1], 2) + std::pow(normalizedVectorOfBackLine[2], 2);

	double squaredMagnitudeOfGroundLineVector = std::pow(normalizedVectorOfGroundLine[0], 2) +
		std::pow(normalizedVectorOfGroundLine[1], 2) + std::pow(normalizedVectorOfGroundLine[2], 2);

	//Perform least squares approximation for approximate intersection of back and ground lines
	double m1[2][2] = {{squaredMagnitudeOfBackLineVector, -((normalizedVectorOfBackLine.t()*normalizedVectorOfGroundLine)[0])},
		{-((normalizedVectorOfGroundLine.t()*normalizedVectorOfBackLine)[0]), squaredMagnitudeOfGroundLineVector}};
	double m2[2][1] = {{(normalizedVectorOfBackLine.t()*(pointOnGroundLine-pointOnBackLine))[0]}, {(normalizedVectorOfGroundLine.t()*(pointOnBackLine-pointOnGroundLine))[0]}};
	double lambda = Mat(Mat(2,2,CV_64F,m1).inv()*Mat(2,1,CV_64F,m2)).at<double>(0, 0);

	Point3d approximateIntersection = pointOnBackLine + lambda * normalizedVectorOfBackLine;
	
	return approximateIntersection;
}

Point3d ScanModel::findLaserPlaneNormalVector(Vec6d backLine, Vec6d groundLine) {
	Vec3d normalizedVectorOfBackLine(backLine[0], backLine[1], backLine[2]);
	Vec3d normalizedVectorOfGroundLine(groundLine[0], groundLine[1], groundLine[2]);
	Vec3d normalVectorOfLaserPlane = normalizedVectorOfBackLine.cross(normalizedVectorOfGroundLine);
	
	return normalVectorOfLaserPlane;
}

vector<Point3d> ScanModel::findObjectLaserIntersections(Plane laserPlane, vector<Point2d> redPtsOnObject) {
	vector<Point2d> undistortedRedPointsOnObject;
	vector<Point3d> redPointsOnObjectInCameraCoords;
	vector<Point3d> redPointsOnObjectInBackWorldCoords;
	Point3d cameraOriginInCameraCoords(0, 0, 0);

	undistortPoints(redPtsOnObject, undistortedRedPointsOnObject, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	this->convertPointsToHomogeneous(undistortedRedPointsOnObject, redPointsOnObjectInCameraCoords);

	double lambda;
	Mat redPointOnObjectInCameraCoords;
	Mat redPointBackWorldCoordMatrix;
	Mat redPointGroundWorldCoordMatrix;
	//Calculate the lambda value of each red point on the back plane and store the Camera coordinate of the red point on the object
	for (int i = 0; i < redPointsOnObjectInCameraCoords.size(); i++) {
		//normal is 3x1 so convert to 1x3. backOrigin is 3x1, Mat(cameraOrigin) is 3x1
		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(Mat(laserPlane.getNormalVector()).t() * (Mat(laserPlane.getPointOnPlane()) - Mat(cameraOriginInCameraCoords)) / 
			(Mat(laserPlane.getNormalVector()).t() * Mat(redPointsOnObjectInCameraCoords.at(i)))).at<double>(0, 0);
		redPointsOnObjectInCameraCoords.at(i) = lambda * redPointsOnObjectInCameraCoords.at(i);
		Mat(redPointsOnObjectInCameraCoords.at(i)).convertTo(redPointOnObjectInCameraCoords, CV_64F);
		redPointBackWorldCoordMatrix = Mat(backExtrinsics->getRotationMatrix().inv() * (redPointOnObjectInCameraCoords -
			backExtrinsics->getTranslationMatrix()));
		redPointGroundWorldCoordMatrix = Mat(groundExtrinsics->getRotationMatrix().inv() * (redPointOnObjectInCameraCoords - 
			groundExtrinsics->getTranslationMatrix()));
		if (redPointBackWorldCoordMatrix.at<double>(2,0) > -0.10) {
			//Point is on or behind the back plane, so don't include it
			continue;
		}

		if (redPointGroundWorldCoordMatrix.at<double>(2,0) > -0.10) {
			//Point is on or behind the ground plane, so don't include it
			continue;
		}
		/*
		If the distance from the point in back coordinates to the back origin
		is greater than the distance from the camera origin to the back origin...
		*/
		if (vectorLength(redPointBackWorldCoordMatrix) > vectorLength(backExtrinsics->getTranslationMatrix())) {
			//Point is an outlier, so don't include it
			continue;
		}

		//None of the conditions are met, so include the point as valid
		redPointsOnObjectInBackWorldCoords.push_back(Point3d(redPointBackWorldCoordMatrix.at<double>(0,0), 
				redPointBackWorldCoordMatrix.at<double>(1,0), redPointBackWorldCoordMatrix.at<double>(2,0)));
	}

	return redPointsOnObjectInBackWorldCoords;
}

double ScanModel::vectorLength(Mat vector) {
	double sumOfSquares = std::pow(vector.at<double>(0,0), 2) + std::pow(vector.at<double>(1,0), 2) + std::pow(vector.at<double>(2,0), 2);
	return std::sqrt(sumOfSquares);
}

bool ScanModel::openFileAndAddHeaders() {
	string fileName = saveDirectory + "\\" + saveFileName + ".wrl";
	outputStream.precision(17);
	try {
		outputStream.open(fileName);

		if (!outputStream.is_open()) {
			return false;
		}

		outputStream << "#VRML V2.0 utf8" << std::endl;
		outputStream << "Shape" << std::endl << "{" << std::endl;
		outputStream << "geometry PointSet" << std::endl << "{" << std::endl;
		outputStream << "coord Coordinate" << std::endl << "{" << std::endl;
		outputStream << "point[" << std::endl;
		return true;
	} catch (std::exception& e) {
		vector<vector<Point3d>>().swap(objectPoints);
		numTimesWritten = 0;
		return false;
	}
}

/*
Method adapted from opencv code, obtained from:
https://github.com/Itseez/opencv/blob/master/modules/contrib/src/spinimages.cpp
*/
bool ScanModel::writeNextObjectPointSet(int objectPointIndex)
{
	try {
		for (int j = 0; j < objectPoints.at(objectPointIndex).size(); j++) {
			outputStream << objectPoints.at(objectPointIndex).at(j).x << " " 
				<< objectPoints.at(objectPointIndex).at(j).y << " " 
				<< objectPoints.at(objectPointIndex).at(j).z << std::endl;
		}
		numTimesWritten++;
		return true;
	} catch (std::exception& e) {
		vector<vector<Point3d>>().swap(objectPoints);
		numTimesWritten = 0;
		return false;
	}
}

bool ScanModel::addFootersAndCloseFile() {
	try {
		outputStream << "]" << std::endl; //point[
		outputStream << "}" << std::endl; //Coordinate{

		outputStream << "}" << std::endl; //PointSet{
		outputStream << "}" << std::endl; //Shape{

		outputStream.close();
		vector<vector<Point3d>>().swap(objectPoints);
		return true;
	} catch (std::exception& e) {
		vector<vector<Point3d>>().swap(objectPoints);
		numTimesWritten = 0;
		return false;
	}
}

void ScanModel::setImageWidth(Mat image) {
	imageWidth = image.cols;
}

int ScanModel::getImageWidth() {
	return imageWidth;
}

void ScanModel::setSaveDirectory(string saveDir) {
	saveDirectory = saveDir;
}

void ScanModel::setLoadDirectory(string loadDir) {
	loadDirectory = loadDir;
}

void ScanModel::setSaveFileName(string fileName) {
	saveFileName = fileName;
}

int ScanModel::setYRegion(int yCoordinate) {
	regionYCoordinates.push_back(yCoordinate);
	return regionYCoordinates.size();
}

int ScanModel::setXRegion(int xCoordinate) {
	regionXCoordinates.push_back(xCoordinate);
	return regionXCoordinates.size();
}

void ScanModel::resetRegions()
{
	vector<int>().swap(regionYCoordinates);
	vector<int>().swap(regionXCoordinates);
}

void ScanModel::sortStoredYCoords() {
	std::sort(regionYCoordinates.begin(), regionYCoordinates.end());

	//After sorted, we know which clicks associate with which regions
	topOfBackPlane = regionYCoordinates[0];
	bottomOfBackPlane = regionYCoordinates[1];
	topOfGroundPlane = regionYCoordinates[2];
	bottomOfGroundPlane = regionYCoordinates[3];

	vector<int>().swap(regionYCoordinates);
}

void ScanModel::sortStoredXCoords() {
	std::sort(regionXCoordinates.begin(), regionXCoordinates.end());

	//After sorted, we know which click is on which side of the object
	leftSideOfObject = regionXCoordinates[0];
	rightSideOfObject = regionXCoordinates[1];

	vector<int>().swap(regionXCoordinates);
}

vector<Point> ScanModel::getRegionYPixels() {
	vector<Point> regionYPixels;
	regionYPixels.push_back(Point(0, topOfBackPlane));
	regionYPixels.push_back(Point(0, bottomOfBackPlane));
	regionYPixels.push_back(Point(0, topOfGroundPlane));
	regionYPixels.push_back(Point(0, bottomOfGroundPlane));
	return regionYPixels;
}

vector<Point> ScanModel::getRegionXPixels() {
	vector<Point> regionXPixels;
	regionXPixels.push_back(Point(0, bottomOfBackPlane));
	regionXPixels.push_back(Point(leftSideOfObject, topOfGroundPlane));
	regionXPixels.push_back(Point(rightSideOfObject, bottomOfBackPlane));
	regionXPixels.push_back(Point(imageWidth, topOfGroundPlane));
	return regionXPixels;
}

int ScanModel::getTopRowToProcess() {
	return topOfBackPlane;
}

int ScanModel::getBottomRowToProcess() {
	return bottomOfGroundPlane;
}

bool ScanModel::loadXML() {
	Mat intrinsicMatrix;
	Mat distortionCoefficients;
	Mat backRotationMatrix;
	Mat groundRotationMatrix;
	Mat backTranslationMatrix;
	Mat groundTranslationMatrix;
	try {
		FileStorage * fs = new FileStorage();
		bool loadOk = fs->open(loadDirectory + "\\intrinsics.xml", FileStorage::READ);
		if (!loadOk) {
			return false;
		}
		(*fs)["intrinsicMatrix"] >> intrinsicMatrix;
		(*fs)["distortionCoefficients"] >> distortionCoefficients;
		fs->release();
		loadOk = fs->open(loadDirectory + "\\extrinsics.xml", FileStorage::READ);
		if (!loadOk) {
			return false;
		}
		(*fs)["backRotationMatrix"] >> backRotationMatrix;
		(*fs)["backTranslationMatrix"] >> backTranslationMatrix;
		(*fs)["groundRotationMatrix"] >> groundRotationMatrix;
		(*fs)["groundTranslationMatrix"] >> groundTranslationMatrix;
		fs->release();
	} catch (cv::Exception& e) {
		return false;
	} catch (std::exception& e) {
		return false;
	}
	intrinsics = new Intrinsic(intrinsicMatrix, distortionCoefficients);
	backExtrinsics = new Extrinsic(backRotationMatrix, backTranslationMatrix);
	groundExtrinsics = new Extrinsic(groundRotationMatrix, groundTranslationMatrix);
	return true;
}

int ScanModel::getRequiredNumStoredYCoords() {
	return REQUIRED_NUM_STORED_Y_COORDS;
}

int ScanModel::getRequiredNumStoredXCoords() {
	return REQUIRED_NUM_STORED_X_COORDS;
}

bool ScanModel::isDoneFindingFindingDifferenceImages() {
	if ((bottomOfGroundPlane - topOfBackPlane) == processedRows) {
		processedRows = 0;
		return true;
	}
	
	return false;
}

bool ScanModel::isDoneFindingRedPoints() {
	
	if (numImages == processedImages) {
		//Free memory from redChannels vector 
		vector<Mat>().swap(redChannels);	
		
		processedImages = 0;
		return true;
	}

	return false;
}

bool ScanModel::isDoneProcessingFrames()
{
	if (numImages == processedImages) {
		vector<vector<Point2d>>().swap(redPointsInBackPlaneLine);
		vector<vector<Point2d>>().swap(redPointsInGroundPlaneLine);
		vector<vector<Point2d>>().swap(redPointsOnObject);
		this->deleteIntrinsicAndExtrinsicMatrices();
		processedImages = 0;
		numImages = 0;
		return true;
	}
	return false;
}

bool ScanModel::isDoneWritingToFile() {
	if (objectPoints.size() == numTimesWritten) {
		vector<vector<Point3d>>().swap(objectPoints);
		numTimesWritten = 0;
		return true;
	}
	return false;
}

void ScanModel::deleteIntrinsicAndExtrinsicMatrices() {
	if(intrinsics)
	{
		if (!intrinsics->getIntrinsicMatrix().empty() &&
			!intrinsics->getDistortionCoefficients().empty())
		{
			delete intrinsics;
		}
	}
	if (backExtrinsics)
	{
		if (!backExtrinsics->getRotationMatrix().empty() &&
			!backExtrinsics->getTranslationMatrix().empty())
		{
			delete backExtrinsics;
		}
	}
	if (groundExtrinsics)
	{
		if (!groundExtrinsics->getRotationMatrix().empty() &&
			!groundExtrinsics->getTranslationMatrix().empty())
		{
			delete groundExtrinsics;
		}
	}
}

ScanModel::~ScanModel()
{
	//Code to delete intrinsic and extrinsic data is now done after the process is done
	//but before writing to the file. The destructor is left in place here.
}