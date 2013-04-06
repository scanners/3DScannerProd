#include "stdafx.h"

#include "scanModel.h"
#include "scanController.h"
#include "scanningView.h"
#include "scanInputView.h"
#include "image.h"
#include "overlayView.h"

ScanController::ScanController() {

}

void ScanController::startScan() {
	/*
	While not done processing frames:
		update progress bar
		process next frame
	*/
	// the above pseudocode will require refactoring!

	scanModel->processScan();
	int currentFrame = 0;
	int totalFrames = scanModel->getNumImages();
	while(!scanModel->isDoneProcessingFrames())
	{
		scanningView->updateProgressBar(currentFrame, totalFrames);
		scanModel->processNextFrame(currentFrame);
		currentFrame++;
	}

	// scanning processing is complete, create point cloud:
	scanModel->createPointCloud();
	scanningView->enableDoneButton(true);
	scanningView->showMessage("Scan Complete. Click \"Done\" to close this window");
}

void ScanController::resetScan() {
	scanModel->resetScan();
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

void ScanController::createOverlayView()
{
	overlayView = new OverlayView();
	overlayView->setScanController(*this);
	overlayView->setModal(true);
	overlayView->show();
}

void ScanController::saveScan() {

}

void ScanController::setRegion(int yCoordinate) {
	int regions = scanModel->setRegion(yCoordinate);
	if (regions == scanModel->getRequiredNumStoredYCoords()) {
		scanModel->sortStoredYCoords();
		overlayView->drawOverlayRegions(scanModel->getRegionPixels(), scanModel->getImageWidth());
	}
}

void ScanController::resetRegions()
{
	scanModel->resetRegions();
}

void ScanController::sendImage(Mat image) {
	scanModel->storeRedChannel(image);
}

void ScanController::setImageWidth(Mat image) {
	scanModel->setImageWidth(image);
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

ScanController::~ScanController()
{
	//// checking objects existence before deallocation
	////if (scanningView) delete scanningView;
	//if (overlayView) delete overlayView;
	//if (scanInputView) delete scanInputView;
	//if (scanModel) delete scanModel;
}