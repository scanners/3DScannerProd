#include "stdafx.h"
#include <exception>
#include "scanModel.h"
#include "intrinsic.h"
#include "extrinsic.h"
#include "image.h"
#include "plane.h"
#include "Serial.h"
#include <Windows.h>
#include <fstream>
using std::ofstream;

ScanModel::ScanModel() : scanComplete(false), processedImages(0), processedRows(0), intrinsics(0), backExtrinsics(0),
groundExtrinsics(0){}

int ScanModel::ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Listener"), MB_ICONSTOP|MB_OK);
	return 1;
}

int ScanModel::scan() {
	CSerial serial;
	LONG lLastError = ERROR_SUCCESS;

	// Attempt to open the serial port (COM1)
	lLastError = serial.Open("COM1",0,0,true);

	if (lLastError != ERROR_SUCCESS)
		return ShowError(serial.GetLastError(), _T("Unable to open COM-port"));
	
	//Setup the Serial Port
	lLastError = serial.Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		return ShowError(serial.GetLastError(), _T("Unable to set COM-port setting"));

	// Setup handshaking
    lLastError = serial.SetupHandshaking(CSerial::EHandshakeSoftware);
	if (lLastError != ERROR_SUCCESS)
		return ShowError(serial.GetLastError(), _T("Unable to set COM-port handshaking"));

	// Register only for the receive event
    lLastError = serial.SetMask(CSerial::EEventError |
								CSerial::EEventRcvEv  |
								CSerial::EEventRecv);
	if (lLastError != ERROR_SUCCESS)
		return ShowError(serial.GetLastError(), _T("Unable to set COM-port event mask"));

	// Use 'non-blocking' reads, because we don't know how many bytes
	// will be received. This is normally the most convenient mode
	// (and also the default mode for reading data).
    lLastError = serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
		return ShowError(serial.GetLastError(), _T("Unable to set COM-port read timeout."));

	lLastError = serial.Write("stop");
	if (lLastError != ERROR_SUCCESS)
		return ShowError(serial.GetLastError(), _T("Unable to send data"));
	
	return 0;
}

bool ScanModel::isDoneScanning(CSerial &serial, LONG &lLastError) {

	string complete = "stop";

	// Create a handle for the overlapped operations
	HANDLE hevtOverlapped = ::CreateEvent(0,TRUE,FALSE,0);;
	if (hevtOverlapped == 0)
		return ShowError(serial.GetLastError(), _T("Unable to create manual-reset event for overlapped I/O."));

	// Setup the overlapped structure
	OVERLAPPED ov = {0};
	ov.hEvent = hevtOverlapped;

	// Open the "STOP" handle
	HANDLE hevtStop = ::CreateEvent(0,TRUE,FALSE,_T("Overlapped_Stop_Event"));
	if (hevtStop == 0)
		return ShowError(serial.GetLastError(), _T("Unable to create manual-reset event for stop event."));

    // Keep reading data, until an EOF (CTRL-Z) has been received
	bool fContinue = true;
	do
	{
		// Wait for an event
		lLastError = serial.WaitEvent(&ov);
		if (lLastError != ERROR_SUCCESS)
			return ShowError(serial.GetLastError(), _T("Unable to wait for a COM-port event."));

		// Setup array of handles in which we are interested
		HANDLE ahWait[2];
		ahWait[0] = hevtOverlapped;
		ahWait[1] = hevtStop;

		// Wait until something happens
		switch (::WaitForMultipleObjects(sizeof(ahWait)/sizeof(*ahWait),ahWait,FALSE,INFINITE))
		{
		case WAIT_OBJECT_0:
			{
				// Save event
				const CSerial::EEvent eEvent = serial.GetEventType();

				// Handle error event
				if (eEvent & CSerial::EEventError)
				{
					printf("\n### ERROR: ");
					switch (serial.GetError())
					{
					case CSerial::EErrorBreak:		printf("Break condition");			break;
					case CSerial::EErrorFrame:		printf("Framing error");			break;
					case CSerial::EErrorIOE:		printf("IO device error");			break;
					case CSerial::EErrorMode:		printf("Unsupported mode");			break;
					case CSerial::EErrorOverrun:	printf("Buffer overrun");			break;
					case CSerial::EErrorRxOver:		printf("Input buffer overflow");	break;
					case CSerial::EErrorParity:		printf("Input parity error");		break;
					case CSerial::EErrorTxFull:		printf("Output buffer full");		break;
					default:						printf("Unknown");					break;
					}
					printf(" ###\n");
				}

				// Handle data receive event
				if (eEvent & CSerial::EEventRecv)
				{
					// Read data, until there is nothing left
					DWORD dwBytesRead = 0;
					do
					{
						char szBuffer[10];

						// Read data from the COM-port
						lLastError = serial.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
						if (lLastError != ERROR_SUCCESS)
							return ShowError(serial.GetLastError(), _T("Unable to read from COM-port."));

						if (dwBytesRead > 0)
						{
							// Finalize the data, so it is a valid string
							szBuffer[dwBytesRead] = '\0';

							if(complete.compare(0, string::npos, szBuffer, 4) == 0){
								fContinue = false;
							}
						}
					}
					while (dwBytesRead > 0);
				}
			}
			break;

		case WAIT_OBJECT_0+1:
			{
				// Set the continue bit to false, so we'll exit
				fContinue = false;
			}
			break;

		default:
			{
				// Something went wrong
				return ShowError(serial.GetLastError(), _T("Error while calling WaitForMultipleObjects."));
			}
			break;
		}
	}
	while (fContinue);

    // Close the port again
    serial.Close();
    return !fContinue;
}

int ScanModel::getNumImages()
{
	return numImages;
}

int ScanModel::getProcessedImages()
{
	return processedImages;
}

void ScanModel::resetScan() {
	//Free memory
	vector<int>().swap(regionYCoordinates);
	vector<Mat>().swap(redChannels);
	vector<vector<Point3d>>().swap(objectPoints);
	vector<vector<Point2d>>().swap(redPointsInBackPlaneLine);
	vector<vector<Point2d>>().swap(redPointsInGroundPlaneLine);
	vector<vector<Point2d>>().swap(redPointsOnObject);
	numImages = 0;
	processedImages = 0;
	processedRows = 0;
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
		if (std::abs(redPointBackWorldCoordMatrix.at<double>(2,0)) < 0.10) {
			//Point is on the back plane, so don't include it
			continue;
		}
		if (std::abs(redPointGroundWorldCoordMatrix.at<double>(2,0)) < 0.10) {
			//Point is on the ground plane, so don't include it
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

/*
Method adapted from opencv code, obtained from:
https://github.com/Itseez/opencv/blob/master/modules/contrib/src/spinimages.cpp
*/
bool ScanModel::createPointCloud()
{
	string fileName = saveDirectory + "\\" + saveFileName + ".wrl";
	ofstream outputStream;
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

		for(int i = 0; i < objectPoints.size(); i++) {
			for (int j = 0; j < objectPoints.at(i).size(); j++) {
				double test = objectPoints.at(i).at(j).y;
				outputStream << objectPoints.at(i).at(j).x << " " << objectPoints.at(i).at(j).y << " " << objectPoints.at(i).at(j).z << std::endl;
			}
		}

		outputStream << "]" << std::endl; //point[
		outputStream << "}" << std::endl; //Coordinate{

		outputStream << "}" << std::endl; //PointSet{
		outputStream << "}" << std::endl; //Shape{

		outputStream.close();
		return true;
	} catch (std::exception& e) {
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
}

void ScanModel::sortStoredXCoords() {
	std::sort(regionXCoordinates.begin(), regionXCoordinates.end());

	//After sorted, we know which click is on which side of the object
	leftSideOfObject = regionXCoordinates[0];
	rightSideOfObject = regionXCoordinates[1];
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
	return (bottomOfGroundPlane - topOfBackPlane) == processedRows;
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
	return numImages == processedImages;
}

ScanModel::~ScanModel()
{
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