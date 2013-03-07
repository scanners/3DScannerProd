#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include <string>
using std::string;

class ScanModel;
class ScanInputView;
class ScanningView;
class Image;

class ScanController {
private:
	ScanningView * scanningView;
	ScanInputView * scanInputView;
	ScanModel * scanModel;
public:
	ScanController();
	void startScan();
	void setSaveDirectory(string saveDir);
	void setLoadDirectory(string loadDir);
	void createScanningView();
	void saveScan();
	void setRegion(int yCoordinate);
	bool savePicture(Image * image);
	bool loadXML();
	void exit();
	void setScanModel(ScanModel& scanMod);
	void setScanInputView(ScanInputView& scanInputView);
};

#endif //SCANCONTROLLER_H