#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include "stdafx.h"
#include <string>
using std::string;
using namespace::cv;

class ScanModel;
class ScanInputView;
class ScanningView;
class Image;
class OverlayView;

class ScanController {
private:
	ScanningView * scanningView;
	OverlayView * overlayView;
	ScanInputView * scanInputView;
	ScanModel * scanModel;
public:
	ScanController();
	~ScanController();
	void processScan();
	void resetScan();
	void setSaveDirectory(string saveDir);
	void setLoadDirectory(string loadDir);
	void createScanningView();
	void createOverlayView();
	void setYRegion(int yCoordinate);
	void setXRegion(int xCoordinate);
	bool isHardwareDoneScanning();
	void storeRedChannel(Mat image);
	void setImageWidth(Mat image);
	bool loadXML();
	void exit();
	void setScanModel(ScanModel& scanMod);
	void setScanInputView(ScanInputView& scanInputView);
	void resetRegions();
	int getRequiredNumStoredYCoords();
	int getRequiredNumStoredXCoords();
};

#endif //SCANCONTROLLER_H