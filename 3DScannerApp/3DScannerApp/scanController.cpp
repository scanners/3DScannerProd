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
		scanModel->findNextDifferenceImage(currentRow);
		scanningView->updateProgressBar(progressBarCounter);
		progressBarCounter++;
		currentRow++;
	}

	while(!scanModel->isDoneFindingRedPoints()) {
		scanningView->showMessage("Finding red points...");
		scanModel->findNextRedPoints(currentFrame);
		scanningView->updateProgressBar(progressBarCounter);
		progressBarCounter++;
		currentFrame++;
	}

	currentFrame = 0;

	while(!scanModel->isDoneProcessingFrames())	{
		scanningView->showMessage("Processing frames...");
		scanModel->processNextFrame(currentFrame);
		scanningView->updateProgressBar(progressBarCounter);
		progressBarCounter++;
		currentFrame++;
	}

	bool openSuccess = scanModel->openFileAndAddHeaders();

	int currentObjectPointIndex = 0;
	//Default to true in case there was no data for the while loop to go through
	bool writeSuccess = true;
	bool closeSuccess = false;

	if (openSuccess) {
		// scanning processing is complete, create point cloud:
		while(!scanModel->isDoneWritingToFile()) {
			scanningView->showMessage("Exporting data...");
			writeSuccess = scanModel->writeNextObjectPointSet(currentObjectPointIndex);
			if (!writeSuccess) {
				break;
			}
			//Used to update frame to show message
			scanningView->updateProgressBar(progressBarCounter);
			currentObjectPointIndex++;
		}
		closeSuccess = scanModel->addFootersAndCloseFile();
		progressBarCounter++;
	}

	if (openSuccess && writeSuccess && closeSuccess) {
		scanningView->updateProgressBar(progressBarCounter);
		scanningView->enableDoneButton(true);
		scanningView->showMessage("Scan Complete. Click \"Done\" to close this window");
	} else {
		scanningView->enableDoneButton(true);
		scanningView->showMessage("There was an error saving the file. Try choosing a different directory or running the program as an Administrator.\n"
			"Click \"Done\" To close this window");
	}
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

	scanModel->waitForHardwareScanComplete();
	scanModel->startHardwareScan();
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

bool ScanController::isHardwareDoneScanning() {
	return scanModel->isHardwareDoneScanning();
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