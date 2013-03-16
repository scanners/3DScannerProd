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
	Point3f cameraOriginInCameraCoords(0, 0, 0);
	Point3f planeOriginInWorldCoords(0, 0, 0);
	Vec3f planeNormalVectorInWorldCoords(0, 1, 0);
	
	vector<Point2f> undistortedImagePoints;
	vector<Point3f> pointsInCameraCoords;
	
	Mat planeOriginInCameraCoords;
	Mat planeNormalVectorInCameraCoords;

	if (boardLocation == Enums::boardLocation::BACK_PLANE) {
		//3x1 Matrices representing Point3f. Convert world to camera
		planeOriginInCameraCoords = backExtrinsics->getRotationMatrix() * Mat(planeOriginInWorldCoords) + backExtrinsics->getTranslationMatrix();
		//3x1 Matrices representing Vec3f. Convert world to camera
		planeNormalVectorInCameraCoords = backExtrinsics->getRotationMatrix() * Mat(planeNormalVectorInWorldCoords) + backExtrinsics->getTranslationMatrix();
	} else if (boardLocation == Enums::boardLocation::GROUND_PLANE) {
		//3x1 Matrices representing Point3f. Convert world to camera
		planeOriginInCameraCoords = groundExtrinsics->getRotationMatrix() * Mat(planeOriginInWorldCoords) + groundExtrinsics->getTranslationMatrix();
		//3x1 Matrices representing Vec3f. Convert world to camera
		planeNormalVectorInCameraCoords = groundExtrinsics->getRotationMatrix() * Mat(planeNormalVectorInWorldCoords) + groundExtrinsics->getTranslationMatrix();
	}

	undistortPoints(imagePoints, undistortedImagePoints, intrinsics->getIntrinsicMatrix(), intrinsics->getDistortionCoefficients());
	convertPointsToHomogeneous(undistortedImagePoints, pointsInCameraCoords);
	
	float lambda;
	
	//Calculate the lambda value of each red point on the plane and store the Camera coordinate of the red point on the back plane
	for (int i = 0; i < pointsInCameraCoords.size(); i++) {
		//normal is 3x1 so convert to 1x3. backOrigin is 3x1, Mat(cameraOrigin) is 3x1
		//The multiplication is ((1x3)*(3x1-3x1)/((1x3)*(3x1)), resulting in a 1x1 matrix, that is, a float at (0,0)
		lambda = Mat(planeNormalVectorInCameraCoords.t() * (planeOriginInCameraCoords - Mat(cameraOriginInCameraCoords)) / 
			(planeNormalVectorInCameraCoords.t() * Mat(pointsInCameraCoords.at(i)))).at<float>(0, 0);
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