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

Plane* ScanModel::getLaserPlane() {
	return NULL;
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
			for (int n = 0; n < redChannels.size(); n++) {
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