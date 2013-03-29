#include "stdafx.h"
#include <exception>
#include "scanModel.h"
#include "intrinsic.h"
#include "extrinsic.h"
#include "image.h"
#include "plane.h"
#include "Serial.h"
#include <Windows.h>

ScanModel::ScanModel() : scanComplete(false) {
}

int __cdecl ScanModel::scan() {
	CSerial serial;
	LONG lLastError = ERROR_SUCCESS;

	// Attempt to open the serial port (COM1)
	lLastError = serial.Open("COM1",0,0,false);

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

// this method needs to be broken up and return an integer
// so the controller can properly update the progress bar
// ScanController will call scanView.updateProgressBar()
// but it needs the number of finished images to do so
void ScanModel::processScan() {
	Plane * laserPlane;
	this->findDifferenceImages();
	this->findRedPoints();
	for (int n = 0; n < numImages; n++) {
		laserPlane = &(this->findLaserPlane(redPointsInBackPlaneLine.at(n), redPointsInGroundPlaneLine.at(n)));
		objectPoints.push_back(this->findObjectLaserIntersections(*laserPlane, redPointsOnObject.at(n)));
	}

	//FOR TESTING OF OUTPUT
	vector<Point3f> pointCloudPoints;
	for (int i = 0; i < objectPoints.size(); i++) {
		for (int j = 0; j < objectPoints.at(i).size(); j++) {
			pointCloudPoints.push_back(objectPoints.at(i).at(j));
		}
	}
	Mesh3D pointCloud(pointCloudPoints);
	pointCloud.writeAsVrml("C:/PointCloud.wrl");
}

void ScanModel::resetScan() {
	//Free memory
	vector<int>().swap(regionYCoordinates);
	vector<Mat>().swap(redChannels);
	vector<vector<Point3f>>().swap(objectPoints);
	vector<vector<Point2f>>().swap(redPointsInBackPlaneLine);
	vector<vector<Point2f>>().swap(redPointsInGroundPlaneLine);
	vector<vector<Point2f>>().swap(redPointsOnObject);
}

void ScanModel::storeRedChannel(Mat image) {
	vector<Mat> channels(image.channels());
	//Split the image into its 3 channels: B, G, R
	split(image, channels);
	//Convert red component uchar matrix to float matrix
	channels[2].convertTo(channels[2], CV_32F);
	redChannels.push_back(channels[2]);
}

void ScanModel::findDifferenceImages() {
	numImages = redChannels.size();

	//Find difference image for the whole section of the image we are using
	for (int x = 0; x < imageWidth; x++) {
		for (int y = topOfBackPlane; y < bottomOfGroundPlane; y++) {
				this->findDifferenceImageAtPixel(x, y, redChannels);
		}
	}
}

void ScanModel::findDifferenceImageAtPixel(int x, int y, vector<Mat>& redChannels) {
	float midpointRedComponent = this->findMidpointRedComponentAtPixel(x, y, redChannels);
	//Compute difference image. 
	for (int n = 0; n < numImages; n++) {
		redChannels.at(n).at<float>(Point(x, y)) = redChannels.at(n).at<float>(Point(x, y)) - midpointRedComponent;
	}
}

float ScanModel::findMidpointRedComponentAtPixel(int x, int y, vector<Mat> redChannels) {
	//Set min to the maximum value so it gets set to a lower value
	float minRedComponent = 255;
	//Set max to the minimum value so it gets set to a higher value
	float maxRedComponent = 0;
	float midpointRedComponent;

	for (int n = 0; n < numImages; n++) {
		if (redChannels.at(n).at<float>(Point(x, y)) < minRedComponent) {
			minRedComponent = redChannels.at(n).at<float>(Point(x, y));
		}
		if (redChannels.at(n).at<float>(Point(x, y)) > maxRedComponent) {
			maxRedComponent = redChannels.at(n).at<float>(Point(x, y));
		}
	}

	midpointRedComponent = (minRedComponent + maxRedComponent) / 2.0;
	return midpointRedComponent;
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
		if (redChannels.at(imageNum).at<float>(Point(x, y)) == 0.0) {
			return Point2f((float)x, (float)y);
		} else if ((redChannels.at(imageNum).at<float>(Point(x, y)) < 0.0) && (redChannels.at(imageNum).at<float>(Point(x + 1, y)) > 0.0)) {
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
	Mat planeNormalVectorInWorldCoords(Vec3d(0, 1, 0));
	
	vector<Point2f> undistortedImagePoints;
	vector<Point3f> pointsInCameraCoords;
	
	Mat planeOriginInCameraCoords;
	Mat planeNormalVectorInCameraCoords;

	if (boardLocation == Enums::boardLocation::BACK_PLANE) {
		//3x1 Matrices representing Point3f. Convert world to camera
		planeOriginInCameraCoords = backExtrinsics->getRotationMatrix() * planeOriginInWorldCoords + backExtrinsics->getTranslationMatrix();
		//3x1 Matrices representing Vec3f. Convert world to camera
		planeNormalVectorInCameraCoords = backExtrinsics->getRotationMatrix() * planeNormalVectorInWorldCoords + backExtrinsics->getTranslationMatrix();
	} else if (boardLocation == Enums::boardLocation::GROUND_PLANE) {
		//3x1 Matrices representing Point3f. Convert world to camera
		planeOriginInCameraCoords = groundExtrinsics->getRotationMatrix() * planeOriginInWorldCoords + groundExtrinsics->getTranslationMatrix();
		//3x1 Matrices representing Vec3f. Convert world to camera
		planeNormalVectorInCameraCoords = groundExtrinsics->getRotationMatrix() * planeNormalVectorInWorldCoords + groundExtrinsics->getTranslationMatrix();
	}

	undistortPoints(imagePoints, undistortedImagePoints, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	convertPointsToHomogeneous(undistortedImagePoints, pointsInCameraCoords);
	
	double lambda;
	Mat pointInCameraCoords;

	//Calculate the lambda value of each red point on the plane and store the Camera coordinate of the red point on the back plane
	for (int i = 0; i < pointsInCameraCoords.size(); i++) {
		Mat(pointsInCameraCoords.at(i)).convertTo(pointInCameraCoords, CV_64F);
		//normal is 3x1 so convert to 1x3. backOrigin is 3x1, Mat(cameraOrigin) is 3x1
		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(planeNormalVectorInCameraCoords.t() * (planeOriginInCameraCoords - cameraOriginInCameraCoords) / 
			(planeNormalVectorInCameraCoords.t() * pointInCameraCoords)).at<double>(0, 0);
		pointsInCameraCoords.at(i) = lambda * pointsInCameraCoords.at(i);
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
		Mat redPointWorldCoordMatrix = Mat(backExtrinsics->getRotationMatrix().t()*redPointOnObjectInCameraCoords-
			backExtrinsics->getRotationMatrix().t()*backExtrinsics->getTranslationMatrix());
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

int ScanModel::ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Listener"), MB_ICONSTOP|MB_OK);
	return 1;
}

bool ScanModel::isDoneScanning(CSerial &serial, LONG &lLastError) {

	bool scanComplete = false;
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
								scanComplete = true;
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
    return 0;
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

bool ScanModel::isDone(int done, int total)
{
	if (done == total)
		return true;
	else
		return false;
}