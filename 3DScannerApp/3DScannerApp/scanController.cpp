#include "stdafx.h"

#include "scanModel.h"
#include "scanController.h"
#include "scanningView.h"
#include "scanInputView.h"
#include "image.h"
#include "overlayView.h"

ScanController::ScanController() {

}

void ScanController::processScan() {
	/*
	While not done processing frames:
		update progress bar
		process next frame
	*/
	// the above pseudocode will require refactoring!

	int currentRow = scanModel->getTopRowToProcess();
	int totalRows = scanModel->getBottomRowToProcess() - currentRow;

	int currentFrame = 0;
	int totalFrames = scanModel->getNumImages();
	int totalOutputFiles = 1;

	//Range includes total rows to find difference images, total images to find
	//red points, and total images to process the results
	scanningView->setProgressBarRange(totalRows + totalFrames + totalFrames + totalOutputFiles);

	int progressBarCounter = 0;

	while(!scanModel->isDoneFindingFindingDifferenceImages()) {
		scanningView->showMessage("Finding difference images...");
		scanningView->updateProgressBar(progressBarCounter);
		scanModel->findNextDifferenceImage(currentRow);
		progressBarCounter++;
		currentRow++;
	}

	while(!scanModel->isDoneFindingRedPoints()) {
		scanningView->showMessage("Finding red points...");
		scanningView->updateProgressBar(progressBarCounter);
		scanModel->findNextRedPoints(currentFrame);
		progressBarCounter++;
		currentFrame++;
	}

	currentFrame = 0;

	while(!scanModel->isDoneProcessingFrames())	{
		scanningView->showMessage("Processing frames...");
		scanningView->updateProgressBar(progressBarCounter);
		scanModel->processNextFrame(currentFrame);
		progressBarCounter++;
		currentFrame++;
	}
	scanningView->showMessage("Exporting data...");
	// scanning processing is complete, create point cloud:
	scanModel->createPointCloud();
	//ProgressBarCounter was incremented in above while loop after updating the progress bar
	scanningView->updateProgressBar(progressBarCounter);
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

void ScanController::setYRegion(int yCoordinate) {
	int regions = scanModel->setYRegion(yCoordinate);
	if (regions == scanModel->getRequiredNumStoredYCoords()) {
		scanModel->sortStoredYCoords();
		overlayView->drawOverlayYRegions(scanModel->getRegionYPixels(), scanModel->getImageWidth());
	}
}

void ScanController::setXRegion(int xCoordinate) {
	int regions = scanModel->setXRegion(xCoordinate);
	if (regions == scanModel->getRequiredNumStoredXCoords()) {
		scanModel->sortStoredXCoords();
		overlayView->drawOverlayXRegions(scanModel->getRegionXPixels());
	}
}

void ScanController::resetRegions()
{
	scanModel->resetRegions();
}

bool ScanController::isDoneScanning() {
	return false;
}

void ScanController::storeRedChannel(Mat image) {
	scanModel->storeRedChannel(image);
}

void ScanController::setImageWidth(Mat image) {
	scanModel->setImageWidth(image);
}

void ScanController::exit() {

}

void ScanController::setScanModel(ScanModel& scanMod) {
	scanModel = &scanMod;
}

void ScanController::setScanInputView(ScanInputView& scanInpView) {
	scanInputView = &scanInpView;
}

int ScanController::getRequiredNumStoredYCoords() {
	return scanModel->getRequiredNumStoredYCoords();
}

int ScanController::getRequiredNumStoredXCoords() {
	return scanModel->getRequiredNumStoredXCoords();
}

void ScanController::setSaveFileName(string name)
{
	scanModel->setSaveFileName(name);
}

ScanController::~ScanController()
{

}