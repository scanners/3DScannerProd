#include "stdafx.h"
#include <exception>
#include "scanModel.h"
#include "intrinsic.h"
#include "extrinsic.h"
#include "image.h"
#include "plane.h"

ScanModel::ScanModel() {
}

void ScanModel::scan() {

}

Plane ScanModel::findLaserPlane(vector<Point2f> backPlanePoints, vector<Point2f> groundPlanePoints) {
	vector<Point2f> undistortedBackPoints;
	vector<Point3f> backPointsInCameraCoords;
	vector<Point2f> undistortedGroundPoints;
	vector<Point3f> groundPointsInCameraCoords;

	Point3f cameraOriginInCameraCoords(0, 0, 0);
	Point3f backOriginInBackWorldCoords(0, 0, 0);
	Vec3f backNormalVectorInBackWorldCoords(0, 1, 0);
	Point3f groundOriginInGroundWorldCoords(0, 0, 0);
	Vec3f groundNormalVectorInGroundWorldCoords(0, 1, 0);

	//3x1 Matrices representing Point3f. Convert world to camera
	Mat backOriginInCameraCoords = backExtrinsics->getRotationMatrix() * Mat(backOriginInBackWorldCoords) + backExtrinsics->getTranslationMatrix();
	Mat groundOriginInCameraCoords = groundExtrinsics->getRotationMatrix() * Mat(groundOriginInGroundWorldCoords) + groundExtrinsics->getTranslationMatrix();

	//3x1 Matrices representing Vec3f. Convert world to camera
	Mat backNormalVectorInCameraCoords = backExtrinsics->getRotationMatrix() * Mat(backNormalVectorInBackWorldCoords) + backExtrinsics->getTranslationMatrix();
	Mat groundNormalVectorInCameraCoords = groundExtrinsics->getRotationMatrix() * Mat(groundNormalVectorInGroundWorldCoords) + groundExtrinsics->getTranslationMatrix();

	//Undistort image points of red points using distortion coefficients and inverse of the intrinsic matrix. Then convert to homogeneous.
	undistortPoints(backPlanePoints, undistortedBackPoints, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	convertPointsToHomogeneous(undistortedBackPoints, backPointsInCameraCoords);
	undistortPoints(groundPlanePoints, undistortedGroundPoints, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	convertPointsToHomogeneous(undistortedGroundPoints, groundPointsInCameraCoords);
	
	float lambda;
	
	//Calculate the lambda value of each red point on the back plane and store the Camera coordinate of the red point on the back plane
	for (int i = 0; i < backPointsInCameraCoords.size(); i++) {
		//normal is 3x1 so convert to 1x3. backOrigin is 3x1, Mat(cameraOrigin) is 3x1
		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(backNormalVectorInCameraCoords.t() * (backOriginInCameraCoords - Mat(cameraOriginInCameraCoords)) / 
			(backNormalVectorInCameraCoords.t() * Mat(backPointsInCameraCoords.at(i)))).at<float>(0, 0);
		backPointsInCameraCoords.at(i) = lambda * backPointsInCameraCoords.at(i);
	}

	//Calculate the lambda value of each red point on the ground plane and store the Camera coordinate of the red point on the back plane
	for (int i = 0; i < groundPointsInCameraCoords.size(); i++) {
		//normal is 3x1 so convert to 1x3. groundOrigin is 3x1, Mat(cameraOrigin) is 3x1
		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(groundNormalVectorInCameraCoords.t() * (groundOriginInCameraCoords - Mat(cameraOriginInCameraCoords)) / 
			(groundNormalVectorInCameraCoords.t() * Mat(groundPointsInCameraCoords.at(i)))).at<float>(0, 0);
		groundPointsInCameraCoords.at(i) = lambda * groundPointsInCameraCoords.at(i);
	}

	//Find best fit line (in camera coordinates) of laser on back plane.
	//The best fit line is a 6f vector consisting of (vx, vy, vy), which is a vector of length one collinear to the line,
	//and (x0, y0, z0), which is a point on the line
	Vec6f backBestFitLine;
	fitLine(backPointsInCameraCoords, backBestFitLine, CV_DIST_L2, 0, 0.01, 0.01);
	Vec3f normalizedVectorOfBackLine(backBestFitLine[0], backBestFitLine[1], backBestFitLine[2]);
	Vec3f pointOnBackLine(backBestFitLine[3], backBestFitLine[4], backBestFitLine[5]);
	//Compute ||v||^2 = v_x^2 + v_y^2 + v_z^2
	float squaredMagnitudeOfNormalizedVectorOfBackLine = normalizedVectorOfBackLine[0]*normalizedVectorOfBackLine[0] +
		normalizedVectorOfBackLine[1]*normalizedVectorOfBackLine[1] + normalizedVectorOfBackLine[2]*normalizedVectorOfBackLine[2];

	//Find best fit line (in camera coordinates) of laser on ground plane.
	//The best fit line is a 6f vector consisting of (vx, vy, vy), which is a vector of length one collinear to the line,
	//and (x0, y0, z0), which is a point on the line
	Vec6f groundBestFitLine;
	fitLine(groundPointsInCameraCoords, groundBestFitLine, CV_DIST_L2, 0, 0.01, 0.01);
	Vec3f normalizedVectorOfGroundLine(groundBestFitLine[0], groundBestFitLine[1], groundBestFitLine[2]);
	Vec3f pointOnGroundLine(groundBestFitLine[3], groundBestFitLine[4], groundBestFitLine[5]);
	float squaredMagnitudeOfNormalizedVectorOfGroundLine = normalizedVectorOfGroundLine[0]*normalizedVectorOfGroundLine[0] +
		normalizedVectorOfGroundLine[1]*normalizedVectorOfGroundLine[1] + normalizedVectorOfGroundLine[2]*normalizedVectorOfGroundLine[2];

	//Perform least squares approximation for approximate intersection of back and ground lines
	float m1[2][2] = {{squaredMagnitudeOfNormalizedVectorOfBackLine, -((normalizedVectorOfBackLine.t()*normalizedVectorOfGroundLine)[0])},
		{-((normalizedVectorOfGroundLine.t()*normalizedVectorOfBackLine)[0]), squaredMagnitudeOfNormalizedVectorOfGroundLine}};
	float m2[2][1] = {{(normalizedVectorOfBackLine.t()*(pointOnGroundLine-pointOnBackLine))[0]}, {(normalizedVectorOfGroundLine.t()*(pointOnBackLine-pointOnGroundLine))[0]}};
	lambda = Mat(Mat(2,2,CV_32F,m1).inv()*Mat(2,1,CV_32F,m2)).at<float>(0, 0);

	Point3f approximateIntersection = pointOnBackLine + lambda * normalizedVectorOfBackLine;
	Vec3f normalVectorOfLaserPlane = normalizedVectorOfBackLine.cross(normalizedVectorOfGroundLine);
	
	return Plane(approximateIntersection, normalVectorOfLaserPlane);
}

vector<Point3f> ScanModel::findObjectLaserIntersections(Plane laserPlane, vector<Point2f> redPtsOnObject) {
	vector<Point3f> undistortedRedPointsOnObject;
	vector<Point3f> redPointsOnObjectInCameraCoords;
	vector<Point3f> redPointsOnObjectInBackWorldCoords;
	Point3f cameraOriginInCameraCoords(0, 0, 0);

	undistortPoints(redPointsOnObject, undistortedRedPointsOnObject, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	convertPointsToHomogeneous(undistortedRedPointsOnObject, redPointsOnObjectInCameraCoords);

	float lambda;
	
	//Calculate the lambda value of each red point on the back plane and store the Camera coordinate of the red point on the object
	for (int i = 0; i < redPointsOnObjectInCameraCoords.size(); i++) {
		//normal is 3x1 so convert to 1x3. backOrigin is 3x1, Mat(cameraOrigin) is 3x1
		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(Mat(laserPlane.getNormalVector()).t() * (Mat(laserPlane.getPointOnPlane()) - Mat(cameraOriginInCameraCoords)) / 
			(Mat(laserPlane.getNormalVector()).t() * Mat(redPointsOnObjectInCameraCoords.at(i)))).at<float>(0, 0);
		redPointsOnObjectInCameraCoords.at(i) = lambda * redPointsOnObjectInCameraCoords.at(i);
		Mat redPointWorldCoordMatrix = Mat(backExtrinsics->getRotationMatrix().t()*Mat(redPointsOnObjectInCameraCoords.at(i))-
			backExtrinsics->getRotationMatrix().t()*backExtrinsics->getTranslationMatrix());
		redPointsOnObjectInBackWorldCoords.push_back(Point3f(redPointWorldCoordMatrix.at<float>(0,0), 
			redPointWorldCoordMatrix.at<float>(1,0), redPointWorldCoordMatrix.at<float>(2,0)));
	}

	return redPointsOnObjectInBackWorldCoords;
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
	regionYCoordinates.clear();
}

int ScanModel::getNumStoredCoords() {
	return 0;
}

vector<int> ScanModel::sortedStoredYCoords() {
	std::sort(regionYCoordinates.begin(), regionYCoordinates.end());
	return regionYCoordinates;
}

void ScanModel::storeRedChannel(Mat image) {
	vector<Mat> channels(image.channels());
	//Split the image into its 3 channels: B, G, R
	split(image, channels);
	//Convert red component uchar matrix to float matrix
	channels[2].convertTo(channels[2], CV_32F);
	redChannels.push_back(channels[2]);
	//For testing
	//findDifferenceImages();
}

void ScanModel::findDifferenceImages() {
	//Set min to the maximum value so it gets set to a lower value
	float minRedComponent = 255;
	//Set max to the minimum value so it gets set to a higher value
	float maxRedComponent = 0;
	float midpointRedComponent;

	imageWidth = redChannels.at(0).cols;
	imageHeight = redChannels.at(0).rows;
	numImages = redChannels.size();

	for (int x = 0; x < imageWidth; x++) {
		for (int y = 0; y < imageHeight; y++) {
			for (int n = 0; n < numImages; n++) {
				if (redChannels.at(n).at<float>(Point(x, y)) < minRedComponent) {
					minRedComponent = redChannels.at(n).at<float>(Point(x, y));
				}
				if (redChannels.at(n).at<float>(Point(x, y)) > maxRedComponent) {
					maxRedComponent = redChannels.at(n).at<float>(Point(x, y));
				}
			}
			midpointRedComponent = (minRedComponent + maxRedComponent) / 2.0;

			//Compute difference image. 
			for (int n = 0; n < numImages; n++) {
				redChannels.at(n).at<float>(Point(x, y)) = redChannels.at(n).at<float>(Point(x, y)) - midpointRedComponent;
			}
			//Reset variables
			minRedComponent = 255;
			maxRedComponent = 0;
		}
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
	float interpolatedX;

	//Set vectors to hold right amount of data for accessing
	redPointsInBackPlaneLine.resize(numImages);
	redPointsInGroundPlaneLine.resize(numImages);
	redPointsOnObject.resize(numImages);

	for (int n = 0; n < numImages; n++) {
		//Find red points on back plane
		for (int y = topOfBackPlane; y < bottomOfBackPlane; y++) {
			//From image left to image right
			for (int x = 0; x < imageWidth - 1; x++) {
				if (redChannels.at(n).at<float>(Point(x, y)) == 0.0) {
					redPointsInBackPlaneLine.at(n).push_back(Point2f((float)x, (float)y));
					break;
				} else if ((redChannels.at(n).at<float>(Point(x, y)) < 0.0) && (redChannels.at(n).at<float>(Point(x + 1, y)) > 0.0)) {
					//Interpolate between x and x + 1 to find the x-value that crosses deltaRed = 0
					//Formula is: x_0 = x + (0 - deltaRed(x))/(deltaRed(x+1) - deltaRed(x))
					interpolatedX = (float)x + (0 - redChannels.at(n).at<float>(Point(x, y))) / 
							(redChannels.at(n).at<float>(Point(x + 1, y)) - redChannels.at(n).at<float>(Point(x, y)));
					redPointsInBackPlaneLine.at(n).push_back(Point2f(interpolatedX, (float)y));
					break;
				}
			}
		}
		//Find red points on ground plane
		for (int y = topOfGroundPlane; y < bottomOfGroundPlane; y++) {
			//From image left to image right
			for (int x = 0; x < imageWidth - 1; x++) {
				if (redChannels.at(n).at<float>(Point(x, y)) == 0.0) {
					redPointsInGroundPlaneLine.at(n).push_back(Point2f((float)x, (float)y));
					break;
				} else if ((redChannels.at(n).at<float>(Point(x, y)) < 0.0) && (redChannels.at(n).at<float>(Point(x + 1, y)) > 0.0)) {
					//Interpolate between x and x + 1 to find the x-value that crosses deltaRed = 0
					//Formula is: x_0 = x + (0 - deltaRed(x))/(deltaRed(x+1) - deltaRed(x))
					interpolatedX = (float)x + (0 - redChannels.at(n).at<float>(Point(x, y))) / 
							(redChannels.at(n).at<float>(Point(x + 1, y)) - redChannels.at(n).at<float>(Point(x, y)));
					redPointsInGroundPlaneLine.at(n).push_back(Point2f(interpolatedX, (float)y));
					break;
				}
			}
		}
		//Find red points on object
		for (int y = bottomOfBackPlane; y < topOfGroundPlane; y++) {
			//From image left to image right
			for (int x = 0; x < imageWidth - 1; x++) {
				if (redChannels.at(n).at<float>(Point(x, y)) == 0.0) {
					redPointsOnObject.at(n).push_back(Point2f((float)x, (float)y));
					break;
				} else if ((redChannels.at(n).at<float>(Point(x, y)) < 0.0) && (redChannels.at(n).at<float>(Point(x + 1, y)) > 0.0)) {
					//Interpolate between x and x + 1 to find the x-value that crosses deltaRed = 0
					//Formula is: x_0 = x + (0 - deltaRed(x))/(deltaRed(x+1) - deltaRed(x))
					interpolatedX = (float)x + (0 - redChannels.at(n).at<float>(Point(x, y))) / 
							(redChannels.at(n).at<float>(Point(x + 1, y)) - redChannels.at(n).at<float>(Point(x, y)));
					redPointsOnObject.at(n).push_back(Point2f(interpolatedX, (float)y));
					break;
				}
			}
		}
	}
	//Free memory from redChannels vector 
	vector<Mat>().swap(redChannels);
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

bool ScanModel::isDoneScanning() {
	return false;
}

int ScanModel::buildImageObjects() {
	return 0;
}

void ScanModel::processImage(int imageNum) {

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