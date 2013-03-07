#include "stdafx.h"
#include <exception>
#include "scanModel.h"
#include "intrinsic.h"
#include "extrinsic.h"
#include "image.h"

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
		FileStorage fs(loadDirectory + "\\intrinsics.xml", FileStorage::READ);
		fs["intrinsicMatrix"] >> intrinsicMatrix;
		fs["distortionCoefficients"] >> distortionCoefficients;
		fs.release();
		FileStorage fs2(loadDirectory + "\\extrinsics.xml", FileStorage::READ);
		fs2["backRotationMatrix"] >> backRotationMatrix;
		fs2["backTranslationMatrix"] >> backTranslationMatrix;
		fs2["groundRotationMatrix"] >> groundRotationMatrix;
		fs2["groundTranslationMatrix"] >> groundTranslationMatrix;
		fs2.release();
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