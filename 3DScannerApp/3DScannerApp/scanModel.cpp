#include "stdafx.h"
#include <exception>
#include "scanModel.h"
#include "intrinsic.h"
#include "extrinsic.h"
#include "image.h"
#include "plane.h"
#include "Serial.h"
#include <Windows.h>
#include <qmessagebox.h>

ScanModel::ScanModel() : scanComplete(false), processedImages(0), processedRows(0) {
}

int ScanModel::showError (LONG lError, char * errorMessage)
{
	QMessageBox messageBox;
	QString message;
	message.append(errorMessage);
	message.append("\n(error code ");
	message.append(lError+")");
	
	messageBox.setIcon(QMessageBox::Information);
	messageBox.setWindowTitle("Serial Communications Error");
	messageBox.setText(message);

	return 1;
}

int ScanModel::initializeSerialPort() {

	lLastError = ERROR_SUCCESS;

	// Attempt to open the serial port (COM1)
	lLastError = serial.Open("COM1",0,0,true);

	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to open COM-port");
	
	//Setup the Serial Port
	lLastError = serial.Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to set COM-port setting");

	// Setup handshaking
    lLastError = serial.SetupHandshaking(CSerial::EHandshakeSoftware);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to set COM-port handshaking");

	// Register only for the receive event
    lLastError = serial.SetMask(CSerial::EEventError |
								CSerial::EEventRecv);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to set COM-port event mask");

	// Use 'non-blocking' reads, because we don't know how many bytes
	// will be received. This is normally the most convenient mode
	// (and also the default mode for reading data).
    lLastError = serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to set COM-port read timeout.");
	
	return 0;
}

int ScanModel::startStepperMotor(){
	lLastError = serial.Write("stop");
	if (lLastError != ERROR_SUCCESS)
		return showError(serial.GetLastError(), "Unable to send data");

	return 0;
}

int ScanModel::isHardwareDoneScanning() {

	char * complete = "stop";

	bool fContinue = true;
	do
	{
		// Wait for an event
		lLastError = serial.WaitEvent();
		if (lLastError != ERROR_SUCCESS)
			return showError(serial.GetLastError(), "Unable to wait for a COM-port event.");

		// Save event
		const CSerial::EEvent eEvent = serial.GetEventType();

		// Handle break event
		if (eEvent & CSerial::EEventBreak)
		{
			printf("\n### BREAK received ###\n");
		}

		// Handle error event
		if (eEvent & CSerial::EEventError)
		{
			QMessageBox messageBox;
			QString message;
			messageBox.setIcon(QMessageBox::Information);
			messageBox.setWindowTitle("Scanning Error");
			message.append("Error occured while scanning.\nError Type: ");

			switch (serial.GetError())
			{
				case CSerial::EErrorBreak:		message.append("Break condition");			break;
				case CSerial::EErrorFrame:		message.append("Framing error");			break;
				case CSerial::EErrorIOE:		message.append("IO device error");			break;
				case CSerial::EErrorMode:		message.append("Unsupported mode");			break;
				case CSerial::EErrorOverrun:	message.append("Buffer overrun");			break;
				case CSerial::EErrorRxOver:		message.append("Input buffer overflow");	break;
				case CSerial::EErrorParity:		message.append("Input parity error");		break;
				case CSerial::EErrorTxFull:		message.append("Output buffer full");		break;
				default:						message.append("Unknown Error");			break;
			}

			messageBox.setText(message);
			messageBox.exec();
			return 1;
		}

		// Handle data receive event
		if (eEvent & CSerial::EEventRecv)
		{
			// Read data, until there is nothing left
			DWORD dwBytesRead = 0;
			char szBuffer[10];
			do
			{
				// Read data from the COM-port
				lLastError = serial.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
				if (lLastError != ERROR_SUCCESS)
					return showError(serial.GetLastError(), "Unable to read from COM-port.");

				if (dwBytesRead > 0)
				{
					// Finalize the data, so it is a valid string
					szBuffer[dwBytesRead] = '\0';

					// Check for Stop Text
					if(strstr(szBuffer, complete) != NULL)
						fContinue = false;
				}
			}
		    while (dwBytesRead == sizeof(szBuffer)-1);
		}
	}
	while (fContinue);

    // Close the port again
    serial.Close();
    return 0;
}

int ScanModel::getNumImages()
{
	return numImages;
}

int ScanModel::getProcessedImages()
{
	return processedImages;
}

void ScanModel::processRedComponent() {
	numImages = redChannels.size();
	//this->findDifferenceImages();
	//this->findRedPoints();
}

// i is the index of the image we are processing
void ScanModel::processNextFrame(int imageNum)
{
	if ((redPointsInBackPlaneLine.at(imageNum).size() > 0) && (redPointsInGroundPlaneLine.at(imageNum).size() > 0)) {
		Plane laserPlane(this->findLaserPlane(redPointsInBackPlaneLine.at(imageNum), redPointsInGroundPlaneLine.at(imageNum)));
		objectPoints.push_back(this->findObjectLaserIntersections(laserPlane, redPointsOnObject.at(imageNum)));
	}
	processedImages++;
}

void ScanModel::createPointCloud()
{
	vector<Point3f> pointCloudPoints;
	for (int i = 0; i < objectPoints.size(); i++) {
		for (int j = 0; j < objectPoints.at(i).size(); j++) {
			pointCloudPoints.push_back(objectPoints.at(i).at(j));
		}
	}
	Mesh3D pointCloud(pointCloudPoints);
	pointCloud.writeAsVrml("C:/PointCloud.wrl");
	// ^^ THIS SHOULD BE USING THE OUTPUT DIRECTORY ^^
}

void ScanModel::resetScan() {
	//Free memory
	vector<int>().swap(regionYCoordinates);
	vector<Mat>().swap(redChannels);
	vector<vector<Point3f>>().swap(objectPoints);
	vector<vector<Point2f>>().swap(redPointsInBackPlaneLine);
	vector<vector<Point2f>>().swap(redPointsInGroundPlaneLine);
	vector<vector<Point2f>>().swap(redPointsOnObject);
	processedImages = 0;
	processedRows = 0;
}

void ScanModel::storeRedChannel(Mat image) {
	vector<Mat> channels(image.channels());
	//Split the image into its 3 channels: B, G, R
	split(image, channels);
	//Convert red component uchar matrix to float matrix
	channels[2].convertTo(channels[2], CV_32F);
	redChannels.push_back(channels[2]);
}

void ScanModel::findNextDifferenceImage(int y) {
	float midpointRedComponent;

	midpointRedComponent = this->findMidpointRedComponentInRow(y);
	for (int x = 0; x < imageWidth; x++) {
		this->findDifferenceImageAtPixel(x, y, midpointRedComponent);
	}
	
	processedRows++;
}

float ScanModel::findMidpointRedComponentInRow(int y) {
	//Set min to the maximum value so it gets set to a lower value
	float minRedComponent = 255;
	//Set max to the minimum value so it gets set to a higher value
	float maxRedComponent = 0;
	float midpointRedComponent;

	for (int x = 0; x < imageWidth; x++) {
		for (int n = 0; n < numImages; n++) {
			if (redChannels.at(n).at<float>(Point(x, y)) < minRedComponent) {
				minRedComponent = redChannels.at(n).at<float>(Point(x, y));
			}
			if (redChannels.at(n).at<float>(Point(x, y)) > maxRedComponent) {
				maxRedComponent = redChannels.at(n).at<float>(Point(x, y));
			}
		}
	}

	midpointRedComponent = (minRedComponent + maxRedComponent) / 2.0;
	return midpointRedComponent;
}

void ScanModel::findDifferenceImageAtPixel(int x, int y, float midpointRedComponent) {
	//Compute difference image. 
	for (int n = 0; n < numImages; n++) {
		redChannels.at(n).at<float>(Point(x, y)) = redChannels.at(n).at<float>(Point(x, y)) - midpointRedComponent;
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
void ScanModel::findRedPoints() {
	for (int n = 0; n < numImages; n++) {
		redPointsInBackPlaneLine.push_back(this->findRedPointsInRegion(Enums::scanRegion::BACK, n));
		redPointsInGroundPlaneLine.push_back(this->findRedPointsInRegion(Enums::scanRegion::GROUND, n));
		redPointsOnObject.push_back(this->findRedPointsInRegion(Enums::scanRegion::OBJECT, n));
	}
	//Free memory from redChannels vector 
	vector<Mat>().swap(redChannels);
}

void ScanModel::findNextRedPoints(int imageNum) {
	redPointsInBackPlaneLine.push_back(this->findRedPointsInRegion(Enums::scanRegion::BACK, imageNum));
	redPointsInGroundPlaneLine.push_back(this->findRedPointsInRegion(Enums::scanRegion::GROUND, imageNum));
	redPointsOnObject.push_back(this->findRedPointsInRegion(Enums::scanRegion::OBJECT, imageNum));
	processedImages++;
}

vector<Point2f> ScanModel::findRedPointsInRegion(int region, int imageNum) {
	vector<Point2f> redPoints;
	
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

	Point2f zeroCrossing;
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

Point2f ScanModel::findZeroCrossingInRow(int y, int imageNum) {
	float interpolatedX;

	for (int x = 0; x < imageWidth - 1; x++) {
		float test = redChannels.at(imageNum).at<float>(Point(x, y));
		if ((redChannels.at(imageNum).at<float>(Point(x, y)) < 0.0) && (redChannels.at(imageNum).at<float>(Point(x + 1, y)) > 0.0)) {
			//Interpolate between x and x + 1 to find the x-value that crosses deltaRed = 0
			//Formula is: x_0 = x + (0 - deltaRed(x))/(deltaRed(x+1) - deltaRed(x))
			interpolatedX = (float)x + (0 - redChannels.at(imageNum).at<float>(Point(x, y))) / 
				(redChannels.at(imageNum).at<float>(Point(x + 1, y)) - redChannels.at(imageNum).at<float>(Point(x, y)));
			return Point2f(interpolatedX, (float)y);
		}
	}
	
	//Zero crossing not found
	return Point2f(-1.0, -1.0);
}

Plane ScanModel::findLaserPlane(vector<Point2f> backPlanePoints, vector<Point2f> groundPlanePoints) {
	//Find red points in camera coords
	vector<Point3f> backPointsInCameraCoords = this->findRayPlaneIntersections(Enums::boardLocation::BACK_PLANE, backPlanePoints);
	vector<Point3f> groundPointsInCameraCoords = this->findRayPlaneIntersections(Enums::boardLocation::GROUND_PLANE, groundPlanePoints);

	//Find best fit line (in camera coordinates) of laser on back plane.
	//The best fit line is a 6f vector consisting of (vx, vy, vy), which is a vector of length one collinear to the line,
	//and (x0, y0, z0), which is a point on the line
	Vec6f backBestFitLine = this->findBestFitRedLine(backPointsInCameraCoords);
	Vec6f groundBestFitLine = this->findBestFitRedLine(groundPointsInCameraCoords);

	//Find approximate intersection and normal of the laser plane to build laser plane object
	Point3f approximateIntersection = this->findLineLineIntersection(backBestFitLine, groundBestFitLine);
	Vec3f normalVectorOfLaserPlane = this->findLaserPlaneNormalVector(backBestFitLine, groundBestFitLine);

	return Plane(approximateIntersection, normalVectorOfLaserPlane);
}

vector<Point3f> ScanModel::findRayPlaneIntersections(int boardLocation, vector<Point2f> imagePoints) {
	Mat cameraOriginInCameraCoords(Point3d(0, 0, 0));
	Mat planeOriginInWorldCoords(Point3d(0, 0, 0));
	Mat planeNormalVectorInWorldCoords(Vec3d(0, 0, 1));
	
	vector<Point2f> undistortedImagePoints;
	vector<Point3f> pointsInCameraCoords;
	
	Mat cameraOriginInWorldCoords;
	Mat planeOriginInCameraCoords;
	Mat planeNormalVectorInCameraCoords;
	
	Point3f test2;
	if (boardLocation == Enums::boardLocation::BACK_PLANE) {
		//3x1 Matrices representing Point3f. Convert camera origin to world coords
		cameraOriginInWorldCoords = Mat(backExtrinsics->getRotationMatrix().inv() * cameraOriginInCameraCoords-
			backExtrinsics->getRotationMatrix().inv() * backExtrinsics->getTranslationMatrix());
	} else if (boardLocation == Enums::boardLocation::GROUND_PLANE) {
		//3x1 Matrices representing Point3f. Convert camera origin to world coords
		cameraOriginInWorldCoords = Mat(groundExtrinsics->getRotationMatrix().inv() * cameraOriginInCameraCoords-
			groundExtrinsics->getRotationMatrix().inv() * groundExtrinsics->getTranslationMatrix());
	}

	undistortPoints(imagePoints, undistortedImagePoints, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	convertPointsToHomogeneous(undistortedImagePoints, pointsInCameraCoords);

	double lambda;
	Mat pointInCameraCoords;
	Mat convertedImagePoint;
	Mat pointOnPlaneInWorldCoords;

	//Calculate the lambda value of each red point on the plane and store the Camera coordinate of the red point on the back plane
	for (int i = 0; i < pointsInCameraCoords.size(); i++) {
		Mat(pointsInCameraCoords.at(i)).convertTo(pointInCameraCoords, CV_64F);
		if (boardLocation == Enums::boardLocation::BACK_PLANE) {
			convertedImagePoint = Mat(backExtrinsics->getRotationMatrix().inv() * pointInCameraCoords);
		} else if (boardLocation == Enums::boardLocation::GROUND_PLANE) {
			convertedImagePoint = Mat(groundExtrinsics->getRotationMatrix().inv() * pointInCameraCoords);
		}

		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(planeNormalVectorInWorldCoords.t() * (planeOriginInWorldCoords - cameraOriginInWorldCoords) / 
			(planeNormalVectorInWorldCoords.t() * convertedImagePoint)).at<double>(0, 0);
		pointInCameraCoords = lambda * pointInCameraCoords;

		pointsInCameraCoords.at(i) = Point3f(pointInCameraCoords.at<double>(0,0), pointInCameraCoords.at<double>(1,0), pointInCameraCoords.at<double>(2,0));
	}

	return pointsInCameraCoords;
}

Vec6f ScanModel::findBestFitRedLine(vector<Point3f> redPointsInCameraCoords) {
	Vec6f bestFitLine;
	fitLine(redPointsInCameraCoords, bestFitLine, CV_DIST_L2, 0, 0.01, 0.01);
	return bestFitLine;
}

Point3f ScanModel::findLineLineIntersection(Vec6f backLine, Vec6f groundLine) {
	Vec3f normalizedVectorOfBackLine(backLine[0], backLine[1], backLine[2]);
	Vec3f pointOnBackLine(backLine[3], backLine[4], backLine[5]);

	Vec3f normalizedVectorOfGroundLine(groundLine[0], groundLine[1], groundLine[2]);
	Vec3f pointOnGroundLine(groundLine[3], groundLine[4], groundLine[5]);

	//Compute ||v||^2 = v_x^2 + v_y^2 + v_z^2
	float squaredMagnitudeOfBackLineVector = normalizedVectorOfBackLine[0]*normalizedVectorOfBackLine[0] +
		normalizedVectorOfBackLine[1]*normalizedVectorOfBackLine[1] + normalizedVectorOfBackLine[2]*normalizedVectorOfBackLine[2];

	float squaredMagnitudeOfGroundLineVector = normalizedVectorOfGroundLine[0]*normalizedVectorOfGroundLine[0] +
		normalizedVectorOfGroundLine[1]*normalizedVectorOfGroundLine[1] + normalizedVectorOfGroundLine[2]*normalizedVectorOfGroundLine[2];

	//Perform least squares approximation for approximate intersection of back and ground lines
	float m1[2][2] = {{squaredMagnitudeOfBackLineVector, -((normalizedVectorOfBackLine.t()*normalizedVectorOfGroundLine)[0])},
		{-((normalizedVectorOfGroundLine.t()*normalizedVectorOfBackLine)[0]), squaredMagnitudeOfGroundLineVector}};
	float m2[2][1] = {{(normalizedVectorOfBackLine.t()*(pointOnGroundLine-pointOnBackLine))[0]}, {(normalizedVectorOfGroundLine.t()*(pointOnBackLine-pointOnGroundLine))[0]}};
	float lambda = Mat(Mat(2,2,CV_32F,m1).inv()*Mat(2,1,CV_32F,m2)).at<float>(0, 0);

	Point3f approximateIntersection = pointOnBackLine + lambda * normalizedVectorOfBackLine;
	
	return approximateIntersection;
}

Point3f ScanModel::findLaserPlaneNormalVector(Vec6f backLine, Vec6f groundLine) {
	Vec3f normalizedVectorOfBackLine(backLine[0], backLine[1], backLine[2]);
	Vec3f normalizedVectorOfGroundLine(groundLine[0], groundLine[1], groundLine[2]);
	Vec3f normalVectorOfLaserPlane = normalizedVectorOfBackLine.cross(normalizedVectorOfGroundLine);
	
	return normalVectorOfLaserPlane;
}

vector<Point3f> ScanModel::findObjectLaserIntersections(Plane laserPlane, vector<Point2f> redPtsOnObject) {
	vector<Point2f> undistortedRedPointsOnObject;
	vector<Point3f> redPointsOnObjectInCameraCoords;
	vector<Point3f> redPointsOnObjectInBackWorldCoords;
	Point3f cameraOriginInCameraCoords(0, 0, 0);

	undistortPoints(redPtsOnObject, undistortedRedPointsOnObject, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	convertPointsToHomogeneous(undistortedRedPointsOnObject, redPointsOnObjectInCameraCoords);

	float lambda;
	Mat redPointOnObjectInCameraCoords;
	//Calculate the lambda value of each red point on the back plane and store the Camera coordinate of the red point on the object
	for (int i = 0; i < redPointsOnObjectInCameraCoords.size(); i++) {
		//normal is 3x1 so convert to 1x3. backOrigin is 3x1, Mat(cameraOrigin) is 3x1
		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(Mat(laserPlane.getNormalVector()).t() * (Mat(laserPlane.getPointOnPlane()) - Mat(cameraOriginInCameraCoords)) / 
			(Mat(laserPlane.getNormalVector()).t() * Mat(redPointsOnObjectInCameraCoords.at(i)))).at<float>(0, 0);
		redPointsOnObjectInCameraCoords.at(i) = lambda * redPointsOnObjectInCameraCoords.at(i);
		Mat(redPointsOnObjectInCameraCoords.at(i)).convertTo(redPointOnObjectInCameraCoords, CV_64F);
		Mat redPointWorldCoordMatrix = Mat(backExtrinsics->getRotationMatrix().inv() * redPointOnObjectInCameraCoords-
			backExtrinsics->getRotationMatrix().inv() * backExtrinsics->getTranslationMatrix());
		redPointsOnObjectInBackWorldCoords.push_back(Point3f(redPointWorldCoordMatrix.at<double>(0,0), 
			redPointWorldCoordMatrix.at<double>(1,0), redPointWorldCoordMatrix.at<double>(2,0)));
	}

	return redPointsOnObjectInBackWorldCoords;
}

void ScanModel::setImageWidth(Mat image) {
	imageWidth = image.cols;
}

int ScanModel::getImageWidth() {
	return imageWidth;
}

void ScanModel::convertCoords() {

}

void ScanModel::setSaveDirectory(string saveDir) {
	saveDirectory = saveDir;
}

void ScanModel::setLoadDirectory(string loadDir) {
	loadDirectory = loadDir;
}

int ScanModel::setRegion(int yCoordinate) {
	regionYCoordinates.push_back(yCoordinate);
	return regionYCoordinates.size();
}

void ScanModel::resetRegions()
{
	vector<int>().swap(regionYCoordinates);
}

int ScanModel::getNumStoredCoords() {
	return 0;
}

void ScanModel::sortStoredYCoords() {
	std::sort(regionYCoordinates.begin(), regionYCoordinates.end());

	//After sorted, we know which clicks associate with which regions
	topOfBackPlane = regionYCoordinates[0];
	bottomOfBackPlane = regionYCoordinates[1];
	topOfGroundPlane = regionYCoordinates[2];
	bottomOfGroundPlane = regionYCoordinates[3];
}

vector<Point> ScanModel::getRegionPixels() {
	vector<Point> regionPixels;
	regionPixels.push_back(Point(0, topOfBackPlane));
	regionPixels.push_back(Point(0, bottomOfBackPlane));
	regionPixels.push_back(Point(0, topOfGroundPlane));
	regionPixels.push_back(Point(0, bottomOfGroundPlane));
	return regionPixels;
}

int ScanModel::getTopRowToProcess() {
	return topOfBackPlane;
}

int ScanModel::getBottomRowToProcess() {
	return bottomOfGroundPlane;
}

bool ScanModel::savePicture(Image * image) {
	return false;
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

int ScanModel::buildImageObjects() {
	return 0;
}

vector<ObjectPoint>* ScanModel::getObjectPoints() {
	return NULL;
}

void ScanModel::saveFile(string fileName) {

}

void ScanModel::exit() {

}

int ScanModel::getRequiredNumStoredYCoords() {
	return REQUIRED_NUM_STORED_Y_COORDS;
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
	if (intrinsics)
		delete intrinsics;
}