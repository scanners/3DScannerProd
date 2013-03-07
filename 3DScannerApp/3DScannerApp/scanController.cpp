#include "stdafx.h"

#include "scanModel.h"
#include "scanController.h"
#include "scanningView.h"
#include "scanInputView.h"
#include "image.h"

ScanController::ScanController() {

}

void ScanController::startScan() {

}

void ScanController::setSaveDirectory(string saveDir) {
	scanModel->setSaveDirectory(saveDir);
}

void ScanController::setLoadDirectory(string loadDir) {
	scanModel->setLoadDirectory(loadDir);
}

bool ScanController::loadXML() {
	return scanModel->loadXML();
}

void ScanController::createScanningView() {
	scanningView = new ScanningView();
	scanningView->setScanController(*this);
	scanningView->setModal(true);
	scanningView->show();
}

void ScanController::saveScan() {

}

void ScanController::sendRegion(float region) {

}

bool ScanController::savePicture(Image * image) {
	return false;
}

void ScanController::exit() {

}

void ScanController::setScanModel(ScanModel& scanMod) {
	scanModel = &scanMod;
}

void ScanController::setScanInputView(ScanInputView& scanInpView) {
	scanInputView = &scanInpView;
}