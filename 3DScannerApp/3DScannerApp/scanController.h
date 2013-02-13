#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include <string>
using std::string;
#include "image.h"
#include "scanModel.h"

class ScanController {
private:
	//HomeView homeView;
	//ResultsView resultsView;
	//OverlayView overlayView;
	//ScanningView scanningView;
	//ScanInputView scanInputView;
	ScanModel scanModel;
public:
	ScanController();
	void startScan();
	void saveFile(string);
	bool loadDirectory(string);
	//void setHomeView(HomeView);
	void sendRegion(float);
	bool savePicture(Image);
	void exit();
	void setScanModel(ScanModel);
};

#endif //SCANCONTROLLER_H