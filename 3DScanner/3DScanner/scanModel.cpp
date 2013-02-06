#include "stdafx.h"

#include "scanModel.h"

ScanModel::ScanModel() {

}

void ScanModel::scan() {

}

Plane* ScanModel::getLaserPlane() {
	return NULL;
}

void ScanModel::convertCoords() {

}

int ScanModel::sendRegion(float yCoord) {
	return 0;
}

int ScanModel::getNumStoredCoords() {
	return 0;
}

vector<float>* ScanModel::sortStoredCoords() {
	return NULL;
}

bool ScanModel::savePicture(Image image) {
	return false;
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

void ScanModel::loadDirectory(string directory) {

}